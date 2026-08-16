#include <gtest/gtest.h>

#include <atomic>
#include <barrier>
#include <chrono>
#include <future>
#include <random>
#include <thread>
#include <vector>

#include "db/datamanager.hpp"

class ConcurrencyTest : public ::testing::Test {
 protected:
  const std::string dbPath = "test_cine.db";

  void SetUp() override {
    SqliteDatabase db{dbPath};
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

    // Insertar datos padre para pruebas concurrentes
    SqliteStatement(db.getDb(), "INSERT INTO cines (id, nombre) VALUES (1, 'Cine Concurrencia');").step();
    SqliteStatement(db.getDb(), "INSERT INTO peliculas (id, titulo, genero, duracion) VALUES (1, 'Concurrency Movie', 'ACCION', 120);").step();
    SqliteStatement(db.getDb(), "INSERT INTO salas (id, cine_id, numero_sala, filas, columnas) VALUES (1, 1, 1, 10, 10);").step();
    SqliteStatement(db.getDb(), "INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (10, 1, 1, datetime('now', '+1 hour'), 7.5);").step();
    SqliteStatement(db.getDb(), "INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (20, 1, 1, datetime('now', '+2 hours'), 7.5);").step();
    SqliteStatement(db.getDb(), "INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (30, 1, 1, datetime('now', '+3 hours'), 8.0);").step();
  }
};

// 1. Contención Extrema: 100 threads compitiendo exactamente por la misma butaca
TEST_F(ConcurrencyTest, ContencionExtrema100HilosMismaButaca) {
  DataManager dm{dbPath};
  const int idSesion = 10;
  const int targetFila = 4;
  const int targetColumna = 4;
  constexpr int NUM_HILOS = 100;

  std::barrier syncPoint(NUM_HILOS);
  std::vector<std::future<int>> futures;
  futures.reserve(NUM_HILOS);

  for (int i = 0; i < NUM_HILOS; ++i) {
    futures.push_back(std::async(std::launch::async, [&dm, idSesion, targetFila, targetColumna, &syncPoint, i]() {
      // Sincronizar todos los hilos para que arranquen en el mismo instante
      syncPoint.arrive_and_wait();
      Reserva r(-1, idSesion, targetFila, targetColumna, "COMPRADO", 1700000000 + i, "Adulto", 7.50f);
      return dm.crearReserva(r);
    }));
  }

  int exitosas = 0;
  int fallidas = 0;

  for (auto& f : futures) {
    auto status = f.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready) << "Deadlock detectado: un hilo no finalizó a tiempo.";
    int res = f.get();
    if (res > 0) {
      exitosas++;
    } else {
      fallidas++;
    }
  }

  // Exactamente 1 reserva debe tener éxito y 99 deben fallar limpiamente
  EXPECT_EQ(exitosas, 1);
  EXPECT_EQ(fallidas, NUM_HILOS - 1);

  // Verificación en base de datos
  auto reservas = dm.obtenerReservasDeSesion(idSesion);
  EXPECT_EQ(reservas.size(), 1u);
  if (!reservas.empty()) {
    EXPECT_EQ(reservas[0].getFila(), targetFila);
    EXPECT_EQ(reservas[0].getColumna(), targetColumna);
  }
}

// 2. Mapeo Simultáneo: 100 threads reservando 100 butacas distintas (sala completa 10x10)
TEST_F(ConcurrencyTest, ReservaMasiva100HilosSinColision) {
  DataManager dm{dbPath};
  const int idSesion = 10;
  constexpr int NUM_HILOS = 100;

  std::barrier syncPoint(NUM_HILOS);
  std::vector<std::future<int>> futures;
  futures.reserve(NUM_HILOS);

  for (int i = 0; i < NUM_HILOS; ++i) {
    const int fila = i / 10;
    const int col = i % 10;
    futures.push_back(std::async(std::launch::async, [&dm, idSesion, fila, col, &syncPoint]() {
      syncPoint.arrive_and_wait();
      Reserva r(-1, idSesion, fila, col, "COMPRADO", 1700000000, "Adulto", 7.50f);
      return dm.crearReserva(r);
    }));
  }

  int exitosas = 0;
  for (auto& f : futures) {
    auto status = f.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready) << "Timeout o deadlock al reservar butacas distintas.";
    if (f.get() > 0) {
      exitosas++;
    }
  }

  EXPECT_EQ(exitosas, 100);
  auto reservas = dm.obtenerReservasDeSesion(idSesion);
  EXPECT_EQ(reservas.size(), 100u);
}

// 3. Rollback Transaccional Atómico Bajo Carrera de Lotes Solapados
TEST_F(ConcurrencyTest, RollbackTransaccionalEnLotesSolapados) {
  DataManager dm{dbPath};
  const int idSesion = 20;

  // Hilo A intenta reservar [ (1,1), (1,2), (1,3) ]
  // Hilo B intenta reservar [ (1,3), (1,4), (1,5) ] -- solapa en (1,3)
  std::barrier syncPoint(2);

  auto futureA = std::async(std::launch::async, [&dm, idSesion, &syncPoint]() {
    syncPoint.arrive_and_wait();
    std::vector<Reserva> loteA = {
        Reserva(-1, idSesion, 1, 1, "COMPRADO", 1700000000, "Adulto", 7.50f),
        Reserva(-1, idSesion, 1, 2, "COMPRADO", 1700000000, "Adulto", 7.50f),
        Reserva(-1, idSesion, 1, 3, "COMPRADO", 1700000000, "Adulto", 7.50f)};
    return dm.crearReservasMultiples(idSesion, loteA);
  });

  auto futureB = std::async(std::launch::async, [&dm, idSesion, &syncPoint]() {
    syncPoint.arrive_and_wait();
    std::vector<Reserva> loteB = {
        Reserva(-1, idSesion, 1, 3, "COMPRADO", 1700000000, "Niño", 5.00f),
        Reserva(-1, idSesion, 1, 4, "COMPRADO", 1700000000, "Niño", 5.00f),
        Reserva(-1, idSesion, 1, 5, "COMPRADO", 1700000000, "Niño", 5.00f)};
    return dm.crearReservasMultiples(idSesion, loteB);
  });

  bool resA = futureA.get();
  bool resB = futureB.get();

  // Exactamente uno debe tener éxito y el otro debe fallar haciendo rollback íntegro
  EXPECT_TRUE((resA && !resB) || (!resA && resB));

  auto reservas = dm.obtenerReservasDeSesion(idSesion);
  // Deben haberse guardado exactamente 3 reservas correspondientes al lote ganador
  EXPECT_EQ(reservas.size(), 3u);

  if (resA) {
    for (const auto& r : reservas) {
      EXPECT_EQ(r.getTipo(), "Adulto");
      EXPECT_FLOAT_EQ(r.getPrecio(), 7.50f);
    }
  } else {
    for (const auto& r : reservas) {
      EXPECT_EQ(r.getTipo(), "Niño");
      EXPECT_FLOAT_EQ(r.getPrecio(), 5.00f);
    }
  }
}

// 4. Concurrencia en Múltiples Sesiones en Paralelo (Aislamiento por Sesión)
TEST_F(ConcurrencyTest, ConcurrenciaMultiplesSesionesAisladas) {
  DataManager dm{dbPath};
  constexpr int NUM_HILOS_POR_SESION = 25;
  const int idSesion1 = 10;
  const int idSesion2 = 30;

  std::barrier syncPoint(NUM_HILOS_POR_SESION * 2);
  std::vector<std::future<int>> futures;
  futures.reserve(NUM_HILOS_POR_SESION * 2);

  // Hilos para sesión 1 (reservan filas 0 a 4)
  for (int i = 0; i < NUM_HILOS_POR_SESION; ++i) {
    int fila = i / 5;
    int col = i % 5;
    futures.push_back(std::async(std::launch::async, [&dm, idSesion1, fila, col, &syncPoint]() {
      syncPoint.arrive_and_wait();
      Reserva r(-1, idSesion1, fila, col, "COMPRADO", 1700000000, "Adulto", 7.50f);
      return dm.crearReserva(r);
    }));
  }

  // Hilos para sesión 2 (reservan las mismas coordenadas pero en distinta sesión)
  for (int i = 0; i < NUM_HILOS_POR_SESION; ++i) {
    int fila = i / 5;
    int col = i % 5;
    futures.push_back(std::async(std::launch::async, [&dm, idSesion2, fila, col, &syncPoint]() {
      syncPoint.arrive_and_wait();
      Reserva r(-1, idSesion2, fila, col, "COMPRADO", 1700000000, "Jubilado", 5.50f);
      return dm.crearReserva(r);
    }));
  }

  int exitosas = 0;
  for (auto& f : futures) {
    if (f.get() > 0) exitosas++;
  }

  EXPECT_EQ(exitosas, NUM_HILOS_POR_SESION * 2);
  EXPECT_EQ(dm.obtenerReservasDeSesion(idSesion1).size(), static_cast<size_t>(NUM_HILOS_POR_SESION));
  EXPECT_EQ(dm.obtenerReservasDeSesion(idSesion2).size(), static_cast<size_t>(NUM_HILOS_POR_SESION));
}
