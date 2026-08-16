#include <gtest/gtest.h>

#include <future>
#include <thread>
#include <vector>

#include "db/datamanager.hpp"

class ConcurrencyTest : public ::testing::Test {
 protected:
  void SetUp() override {
    SqliteDatabase db{"test_cine.db"};
    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS cines ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, nombre TEXT NOT NULL, direccion TEXT);").step();
    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS peliculas ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, titulo TEXT NOT NULL, genero TEXT NOT NULL, duracion INTEGER NOT NULL);").step();
    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS salas ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, cine_id INTEGER NOT NULL, numero_sala INTEGER NOT NULL, filas INTEGER NOT NULL, columnas INTEGER NOT NULL);").step();
    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS sesiones ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, pelicula_id INTEGER NOT NULL, sala_id INTEGER NOT NULL, fecha_hora TEXT NOT NULL, precio_entrada REAL NOT NULL);").step();
    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS reservas ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, sesion_id INTEGER NOT NULL, fila INTEGER NOT NULL, columna INTEGER NOT NULL, estado TEXT NOT NULL DEFAULT 'PENDIENTE', timestamp_creacion INTEGER NOT NULL DEFAULT 0, tipo TEXT, precio REAL, UNIQUE(sesion_id, fila, columna));").step();

    // Limpiar tablas
    SqliteStatement(db.getDb(), "DELETE FROM reservas;").step();
    SqliteStatement(db.getDb(), "DELETE FROM sesiones;").step();
    SqliteStatement(db.getDb(), "DELETE FROM salas;").step();
    SqliteStatement(db.getDb(), "DELETE FROM peliculas;").step();
    SqliteStatement(db.getDb(), "DELETE FROM cines;").step();

    // Insertar datos padre para sesiones 10 y 20
    SqliteStatement(db.getDb(), "INSERT INTO cines (id, nombre) VALUES (1, 'Cine Test');").step();
    SqliteStatement(db.getDb(), "INSERT INTO peliculas (id, titulo, genero, duracion) VALUES (1, 'Film Test', 'ACCION', 120);").step();
    SqliteStatement(db.getDb(), "INSERT INTO salas (id, cine_id, numero_sala, filas, columnas) VALUES (1, 1, 1, 10, 10);").step();
    SqliteStatement(db.getDb(), "INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (10, 1, 1, datetime('now'), 7.5);").step();
    SqliteStatement(db.getDb(), "INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (20, 1, 1, datetime('now'), 7.5);").step();
  }
};

TEST_F(ConcurrencyTest, ReservasConcurrentesMismoAsiento) {
  DataManager dm{"test_cine.db"};
  int idSesion = 10;
  int fila = 2;
  int columna = 3;

  constexpr int NUM_HILOS = 10;
  std::vector<std::future<int>> futures;
  futures.reserve(NUM_HILOS);

  for (int i = 0; i < NUM_HILOS; ++i) {
    futures.push_back(std::async(std::launch::async, [&dm, idSesion, fila, columna]() {
      Reserva r(-1, idSesion, fila, columna, "COMPRADO", 1700000000, "Adulto", 7.50f);
      return dm.crearReserva(r);
    }));
  }

  int reservasExitosas = 0;
  int reservasFallidas = 0;

  for (auto& f : futures) {
    int res = f.get();
    if (res > 0) {
      reservasExitosas++;
    } else {
      reservasFallidas++;
    }
  }

  // Exactamente un hilo debe tener éxito y el resto debe ser rechazado
  EXPECT_EQ(reservasExitosas, 1);
  EXPECT_EQ(reservasFallidas, NUM_HILOS - 1);
}

TEST_F(ConcurrencyTest, MultiplesReservasAtomicasRollback) {
  DataManager dm{"test_cine.db"};
  int idSesion = 20;

  // Creamos un asiento reservado previamente (fila 0, col 0)
  Reserva ocupado(-1, idSesion, 0, 0, "COMPRADO", 1700000000, "Adulto", 7.50f);
  ASSERT_GT(dm.crearReserva(ocupado), 0);

  // Intentamos reservar un lote de 3 asientos: (0, 1), (0, 2) y el ya ocupado (0, 0)
  std::vector<Reserva> lote = {
      Reserva(-1, idSesion, 0, 1, "COMPRADO", 1700000000, "Adulto", 7.50f),
      Reserva(-1, idSesion, 0, 2, "COMPRADO", 1700000000, "Adulto", 7.50f),
      Reserva(-1, idSesion, 0, 0, "COMPRADO", 1700000000, "Adulto", 7.50f)};

  bool exito = dm.crearReservasMultiples(idSesion, lote);
  EXPECT_FALSE(exito);

  // Verificamos que debido al rollback atómico, (0, 1) y (0, 2) NO fueron creados
  auto reservasActuales = dm.obtenerReservasDeSesion(idSesion);
  EXPECT_EQ(reservasActuales.size(), 1u);
  EXPECT_EQ(reservasActuales[0].getFila(), 0);
  EXPECT_EQ(reservasActuales[0].getColumna(), 0);
}
