#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <future>
#include <thread>
#include <vector>

#include "db/datamanager.hpp"

class ExpirationToctouTest : public ::testing::Test {
 protected:
  const std::string dbPath = "test_toctou.db";

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

    // Insertar datos de sesión base
    SqliteStatement(db.getDb(), "INSERT INTO cines (id, nombre) VALUES (1, 'Cine TOCTOU');").step();
    SqliteStatement(db.getDb(), "INSERT INTO peliculas (id, titulo, genero, duracion) VALUES (1, 'Expiration Movie', 'DRAMA', 100);").step();
    SqliteStatement(db.getDb(), "INSERT INTO salas (id, cine_id, numero_sala, filas, columnas) VALUES (1, 1, 1, 5, 5);").step();
    SqliteStatement(db.getDb(), "INSERT INTO sesiones (id, pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (1, 1, 1, datetime('now', '+2 hours'), 8.0);").step();
  }
};

// 1. Ciclo de vida y parada limpia del hilo cleanerThread
TEST_F(ExpirationToctouTest, CicloDeVidaYParadaLimpiaDemonio) {
  // Comprobar creación y destrucción rápida sin leaks ni deadlocks
  for (int i = 0; i < 5; ++i) {
    auto dm = std::make_unique<DataManager>(dbPath);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    dm.reset();  // Invoca el destructor que debe solicitar stop y join al jthread limpiamente
  }
  SUCCEED();
}

// 2. Expiración de reservas PENDIENTES tras superar el límite de tiempo
TEST_F(ExpirationToctouTest, PurgaAutomaticaDeReservasExpiradas) {
  const std::time_t ahora = std::time(nullptr);

  {
    // Insertamos directamente en base de datos una reserva pendiente caducada y otra activa
    SqliteDatabase db{dbPath};
    ReservaRepository repo{db};

    // Reserva 1: PENDIENTE y creada hace 350 segundos (> 300s -> DEBE PURGARSE)
    Reserva rExpirada(-1, 1, 0, 0, "PENDIENTE", ahora - 350, "Adulto", 8.0f);
    int idExpirada = repo.crear(rExpirada);
    ASSERT_GT(idExpirada, 0);

    // Reserva 2: PENDIENTE y creada hace 30 segundos (< 300s -> DEBE PERMANECER)
    Reserva rActiva(-1, 1, 0, 1, "PENDIENTE", ahora - 30, "Adulto", 8.0f);
    int idActiva = repo.crear(rActiva);
    ASSERT_GT(idActiva, 0);

    // Reserva 3: COMPRADO y creada hace 500 segundos (> 300s pero COMPRADO -> DEBE PERMANECER)
    Reserva rComprada(-1, 1, 0, 2, "COMPRADO", ahora - 500, "Adulto", 8.0f);
    int idComprada = repo.crear(rComprada);
    ASSERT_GT(idComprada, 0);
  }

  // Instanciamos DataManager cuyo demonio ejecutará el ciclo de limpieza (esperamos 5.5s para 1 ciclo)
  {
    DataManager dm{dbPath};
    std::this_thread::sleep_for(std::chrono::milliseconds(5500));

    // Validar estado de las reservas
    auto pendientes = dm.obtenerPendientes();
    EXPECT_EQ(pendientes.size(), 1u);
    if (!pendientes.empty()) {
      EXPECT_EQ(pendientes[0].getColumna(), 1);  // Solo rActiva debe quedar
    }

    // La reserva comprada debe seguir existiendo intacta
    Reserva compradaRec = dm.obtenerReserva(3);
    EXPECT_TRUE(compradaRec.esValido());
    EXPECT_EQ(compradaRec.getEstado(), "COMPRADO");

    // La butaca (0, 0) que expiró debe estar libre para volverse a reservar
    Reserva nuevaReserva(-1, 1, 0, 0, "COMPRADO", std::time(nullptr), "Niño", 5.0f);
    int idNueva = dm.crearReserva(nuevaReserva);
    EXPECT_GT(idNueva, 0) << "La butaca liberada por expiración debe poder ser reservada de nuevo.";
  }
}

// 3. Carrera TOCTOU: Reserva concurrente durante la purga del demonio
TEST_F(ExpirationToctouTest, CarreraTOCTOUReservaYPurga) {
  const std::time_t ahora = std::time(nullptr);
  const int idSesion = 1;
  const int fila = 2;
  const int col = 2;

  // Insertar reserva caducada en (2,2)
  {
    SqliteDatabase db{dbPath};
    ReservaRepository repo{db};
    Reserva rCaducada(-1, idSesion, fila, col, "PENDIENTE", ahora - 400, "Adulto", 8.0f);
    ASSERT_GT(repo.crear(rCaducada), 0);
  }

  DataManager dm{dbPath};

  // Lanzar múltiples hilos que intentan reservar esa misma butaca
  // mientras el demonio está corriendo para verificar que nunca hay duplicados ni inconsistencias
  constexpr int NUM_HILOS = 30;
  std::vector<std::future<int>> futures;
  futures.reserve(NUM_HILOS);

  for (int i = 0; i < NUM_HILOS; ++i) {
    futures.push_back(std::async(std::launch::async, [&dm, idSesion, fila, col, i]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(10 * i));
      Reserva r(-1, idSesion, fila, col, "COMPRADO", std::time(nullptr), "Adulto", 8.0f);
      return dm.crearReserva(r);
    }));
  }

  int exitosas = 0;
  for (auto& f : futures) {
    if (f.get() > 0) exitosas++;
  }

  // Esperar a que el demonio complete su ciclo si hiciera falta
  std::this_thread::sleep_for(std::chrono::milliseconds(5500));

  auto reservasSesion = dm.obtenerReservasDeSesion(idSesion);
  // En ningún caso debe haber más de 1 reserva para esa butaca
  int conteoButaca = 0;
  for (const auto& r : reservasSesion) {
    if (r.getFila() == fila && r.getColumna() == col) {
      conteoButaca++;
    }
  }

  EXPECT_LE(conteoButaca, 1);
}
