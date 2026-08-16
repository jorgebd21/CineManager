#include <gtest/gtest.h>

#include "db/database.hpp"
#include "db/repositories/reservarepository.hpp"
#include "models/reserva.hpp"

class ReservaRepoTest : public ::testing::Test {
 protected:
  SqliteDatabase db{"test_cine.db"};
  ReservaRepository repo{db};

  void SetUp() override {
    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS cines ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, nombre TEXT NOT NULL, direccion TEXT);").step();
    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS peliculas ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, titulo TEXT NOT NULL, genero TEXT NOT NULL, duracion INTEGER NOT NULL);").step();
    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS salas ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, cine_id INTEGER NOT NULL, numero_sala INTEGER NOT NULL, filas INTEGER NOT NULL, columnas INTEGER NOT NULL);").step();
    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS sesiones ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, pelicula_id INTEGER NOT NULL, sala_id INTEGER NOT NULL, fecha_hora TEXT NOT NULL, precio_entrada REAL NOT NULL);").step();
    (void)SqliteStatement(db.getDb(),
                          "CREATE TABLE IF NOT EXISTS reservas ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, sesion_id INTEGER NOT NULL, fila INTEGER NOT NULL, columna INTEGER NOT NULL, estado TEXT NOT NULL DEFAULT 'PENDIENTE', timestamp_creacion INTEGER NOT NULL DEFAULT 0, tipo TEXT, precio REAL, UNIQUE(sesion_id, fila, columna));").step();

    // Limpiar tablas
    (void)SqliteStatement(db.getDb(), "DELETE FROM reservas;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM sesiones;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM salas;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM peliculas;").step();
    (void)SqliteStatement(db.getDb(), "DELETE FROM cines;").step();

    // Insertar datos de prueba para sesiones 1 y 2
    (void)SqliteStatement(db.getDb(), "INSERT INTO cines (id, nombre) VALUES (1, 'Cine Test');").step();
    (void)SqliteStatement(db.getDb(), "INSERT INTO peliculas (id, titulo, genero, duracion) VALUES (1, 'Film Test', 'ACCION', 120);").step();
    (void)SqliteStatement(db.getDb(), "INSERT INTO salas (id, cine_id, numero_sala, filas, columnas) VALUES (1, 1, 1, 5, 5);").step();
    (void)SqliteStatement(db.getDb(), "INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (1, 1, 1, datetime('now'), 7.5);").step();
    (void)SqliteStatement(db.getDb(), "INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (2, 1, 1, datetime('now'), 7.5);").step();
  }
};

TEST_F(ReservaRepoTest, CrearReservaConTarifasDinamicas) {
  Reserva r1(-1, 1, 0, 0, "COMPRADO", 1700000000, "Adulto", 7.50f);
  Reserva r2(-1, 1, 0, 1, "COMPRADO", 1700000000, "Niño", 5.00f);
  Reserva r3(-1, 1, 0, 2, "COMPRADO", 1700000000, "Jubilado", 5.50f);

  int id1 = repo.crear(r1);
  int id2 = repo.crear(r2);
  int id3 = repo.crear(r3);

  EXPECT_GT(id1, 0);
  EXPECT_GT(id2, 0);
  EXPECT_GT(id3, 0);

  Reserva rec1 = repo.obtenerPorId(id1);
  EXPECT_TRUE(rec1.esValido());
  EXPECT_EQ(rec1.getTipo(), "Adulto");
  EXPECT_FLOAT_EQ(rec1.getPrecio(), 7.50f);

  Reserva rec2 = repo.obtenerPorId(id2);
  EXPECT_TRUE(rec2.esValido());
  EXPECT_EQ(rec2.getTipo(), "Niño");
  EXPECT_FLOAT_EQ(rec2.getPrecio(), 5.00f);
}

TEST_F(ReservaRepoTest, RestriccionButacaDuplicada) {
  Reserva r1(-1, 2, 3, 3, "COMPRADO", 1700000000, "Adulto", 7.50f);
  Reserva r2Duplicada(-1, 2, 3, 3, "COMPRADO", 1700000005, "Estudiante", 5.50f);

  int id1 = repo.crear(r1);
  EXPECT_GT(id1, 0);

  // La segunda reserva en la misma sesión, fila y columna debe fallar por el UNIQUE constraint
  int id2 = repo.crear(r2Duplicada);
  EXPECT_EQ(id2, -1);
}

TEST_F(ReservaRepoTest, ObtenerPorSesionYFiltrarPendientes) {
  Reserva rComprada(-1, 1, 1, 1, "COMPRADO", 1700000000, "Adulto", 7.50f);
  Reserva rPendiente1(-1, 1, 1, 2, "PENDIENTE", 1700000010, "Niño", 5.00f);
  Reserva rPendiente2(-1, 2, 0, 0, "PENDIENTE", 1700000020, "Jubilado", 5.50f);

  EXPECT_GT(repo.crear(rComprada), 0);
  EXPECT_GT(repo.crear(rPendiente1), 0);
  EXPECT_GT(repo.crear(rPendiente2), 0);

  // Reservas de la sesión 1
  auto reservasSesion1 = repo.obtenerPorSesion(1);
  EXPECT_EQ(reservasSesion1.size(), 2u);

  // Total de reservas pendientes en el sistema
  auto pendientes = repo.obtenerPendientes();
  EXPECT_EQ(pendientes.size(), 2u);
}

TEST_F(ReservaRepoTest, ActualizarYEliminarReserva) {
  Reserva r(-1, 1, 2, 2, "PENDIENTE", 1700000000, "Adulto", 7.50f);
  int id = repo.crear(r);
  ASSERT_GT(id, 0);

  // Confirmar reserva
  r.setEstado("COMPRADO");
  r.setTipo("Estudiante");
  r.setPrecio(6.00f);
  EXPECT_TRUE(repo.actualizar(id, r));

  Reserva actualizada = repo.obtenerPorId(id);
  EXPECT_EQ(actualizada.getEstado(), "COMPRADO");
  EXPECT_EQ(actualizada.getTipo(), "Estudiante");
  EXPECT_FLOAT_EQ(actualizada.getPrecio(), 6.00f);

  // Eliminar
  EXPECT_TRUE(repo.eliminar(id));
  Reserva eliminada = repo.obtenerPorId(id);
  EXPECT_FALSE(eliminada.esValido());
}

TEST_F(ReservaRepoTest, ObtenerPorIdInexistente) {
  Reserva inexistente = repo.obtenerPorId(99999);
  EXPECT_FALSE(inexistente.esValido());
}
