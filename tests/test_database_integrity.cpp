#include <gtest/gtest.h>

#include "db/database.hpp"
#include "db/repositories/cinerepository.hpp"
#include "db/repositories/pelicularepository.hpp"
#include "db/repositories/reservarepository.hpp"
#include "db/repositories/salarepository.hpp"
#include "db/repositories/sesionrepository.hpp"
#include "db/repositories/usuariorepository.hpp"

class DatabaseIntegrityTest : public ::testing::Test {
 protected:
  const std::string dbPath = "test_integrity.db";
  SqliteDatabase db{dbPath};
  CineRepository cineRepo{db};
  PeliculaRepository peliculaRepo{db};
  SalaRepository salaRepo{db};
  SesionRepository sesionRepo{db};
  ReservaRepository reservaRepo{db};
  UsuarioRepository usuarioRepo{db};

  void SetUp() override {
    // Definición de tablas completas con claves foráneas activas y borrado en cascada
    SqliteStatement(db.getDb(), "PRAGMA foreign_keys = ON;").step();

    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS usuarios ("
                    "dni TEXT PRIMARY KEY, "
                    "nombre TEXT NOT NULL, "
                    "apellidos TEXT NOT NULL, "
                    "email TEXT NOT NULL UNIQUE, "
                    "password_hash TEXT NOT NULL, "
                    "rol TEXT NOT NULL DEFAULT 'CLIENTE' CHECK (rol IN ('CLIENTE', 'ADMIN')));").step();

    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS cines ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "nombre TEXT NOT NULL, "
                    "direccion TEXT);").step();

    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS peliculas ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "titulo TEXT NOT NULL, "
                    "genero TEXT NOT NULL, "
                    "duracion INTEGER NOT NULL);").step();

    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS salas ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "cine_id INTEGER NOT NULL, "
                    "numero_sala INTEGER NOT NULL, "
                    "filas INTEGER NOT NULL, "
                    "columnas INTEGER NOT NULL, "
                    "FOREIGN KEY (cine_id) REFERENCES cines(id) ON DELETE CASCADE);").step();

    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS sesiones ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "pelicula_id INTEGER NOT NULL, "
                    "sala_id INTEGER NOT NULL, "
                    "fecha_hora TEXT NOT NULL, "
                    "precio_entrada REAL NOT NULL, "
                    "FOREIGN KEY (pelicula_id) REFERENCES peliculas(id) ON DELETE CASCADE, "
                    "FOREIGN KEY (sala_id) REFERENCES salas(id) ON DELETE CASCADE);").step();

    SqliteStatement(db.getDb(),
                    "CREATE TABLE IF NOT EXISTS reservas ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "sesion_id INTEGER NOT NULL, "
                    "fila INTEGER NOT NULL, "
                    "columna INTEGER NOT NULL, "
                    "estado TEXT NOT NULL DEFAULT 'PENDIENTE', "
                    "timestamp_creacion INTEGER NOT NULL DEFAULT 0, "
                    "tipo TEXT, "
                    "precio REAL, "
                    "FOREIGN KEY (sesion_id) REFERENCES sesiones(id) ON DELETE CASCADE, "
                    "UNIQUE(sesion_id, fila, columna));").step();

    // Limpiar tablas
    SqliteStatement(db.getDb(), "DELETE FROM reservas;").step();
    SqliteStatement(db.getDb(), "DELETE FROM sesiones;").step();
    SqliteStatement(db.getDb(), "DELETE FROM salas;").step();
    SqliteStatement(db.getDb(), "DELETE FROM peliculas;").step();
    SqliteStatement(db.getDb(), "DELETE FROM cines;").step();
    SqliteStatement(db.getDb(), "DELETE FROM usuarios;").step();
  }
};

// 1. Verificación del modo WAL
TEST_F(DatabaseIntegrityTest, VerificacionJournalModeWAL) {
  SqliteStatement stmt(db.getDb(), "PRAGMA journal_mode;");
  ASSERT_EQ(stmt.step(), SQLITE_ROW);
  std::string mode = stmt.getColumnText(0);
  EXPECT_EQ(mode, "wal");
}

// 2. Cumplimiento estricto de Foreign Keys: Rechazo de inserciones huérfanas
TEST_F(DatabaseIntegrityTest, RechazoInsercionConForeignKeysInvalidas) {
  // a) Sala con cine_id inexistente
  Sala salaHuerfana(-1, 99999, 1, 5, 5);
  int idSala = salaRepo.crear(salaHuerfana);
  EXPECT_EQ(idSala, -1) << "SQLite debe rechazar una sala con cine_id inexistente.";

  // b) Sesión con película o sala inexistente
  Pelicula peliInexistente(99999, "Fantasma", Genero::ACCION, 100);
  Sesion sesionHuerfana(-1, peliInexistente, 1, 1700000000);
  int idSesion = sesionRepo.crear(sesionHuerfana);
  EXPECT_EQ(idSesion, -1) << "SQLite debe rechazar una sesión con película inexistente.";

  // c) Reserva con sesion_id inexistente
  Reserva reservaHuerfana(-1, 99999, 0, 0, "COMPRADO", 1700000000, "Adulto", 7.5f);
  int idReserva = reservaRepo.crear(reservaHuerfana);
  EXPECT_EQ(idReserva, -1) << "SQLite debe rechazar una reserva con sesion_id inexistente.";
}

// 3. Integridad en borrado en cascada (ON DELETE CASCADE)
TEST_F(DatabaseIntegrityTest, BorradoEnCascadaCompleto) {
  // Crear jerarquía: Cine -> Sala -> Pelicula -> Sesion -> Reserva
  int idCine = cineRepo.crear(Cine(-1, "Cine Cascada", "Calle 123"));
  ASSERT_GT(idCine, 0);

  int idSala = salaRepo.crear(Sala(-1, idCine, 1, 5, 5));
  ASSERT_GT(idSala, 0);

  int idPeli = peliculaRepo.crear(Pelicula(-1, "Peli Cascada", Genero::DRAMA, 120));
  ASSERT_GT(idPeli, 0);

  Pelicula peli(idPeli, "Peli Cascada", Genero::DRAMA, 120);
  int idSesion = sesionRepo.crear(Sesion(-1, peli, idSala, 1700000000));
  ASSERT_GT(idSesion, 0);

  int idReserva1 = reservaRepo.crear(Reserva(-1, idSesion, 0, 0, "COMPRADO", 1700000000, "Adulto", 7.5f));
  int idReserva2 = reservaRepo.crear(Reserva(-1, idSesion, 0, 1, "COMPRADO", 1700000000, "Niño", 5.0f));
  ASSERT_GT(idReserva1, 0);
  ASSERT_GT(idReserva2, 0);

  // Eliminar el Cine raíz
  EXPECT_TRUE(cineRepo.eliminar(idCine));

  // Verificar que la cascada eliminó salas, sesiones y reservas dependientes
  EXPECT_FALSE(salaRepo.obtenerPorId(idSala).esValido());
  EXPECT_FALSE(sesionRepo.obtenerPorId(idSesion).esValido());
  EXPECT_FALSE(reservaRepo.obtenerPorId(idReserva1).esValido());
  EXPECT_FALSE(reservaRepo.obtenerPorId(idReserva2).esValido());

  // La película del catálogo debe conservarse pues no dependía del cine
  EXPECT_TRUE(peliculaRepo.obtenerPorId(idPeli).esValido());
}

// 4. Atomicidad de SqliteTransaction: Commit exitoso vs Rollback automático
TEST_F(DatabaseIntegrityTest, SqliteTransactionRollbackYCommit) {
  // Test Commit
  {
    SqliteTransaction tx(db.getDb());
    Usuario u1("11111111A", "Test1", "User", "test1@db.com", "pass1");
    EXPECT_TRUE(usuarioRepo.crear(u1));
    EXPECT_TRUE(tx.commit());
  }
  EXPECT_TRUE(usuarioRepo.obtenerPorDni("11111111A").esValido());

  // Test Rollback al salir del ámbito sin commit (RAII)
  {
    SqliteTransaction tx(db.getDb());
    Usuario u2("22222222B", "Test2", "User", "test2@db.com", "pass2");
    EXPECT_TRUE(usuarioRepo.crear(u2));
    // No llamamos a tx.commit() -> el destructor ejecuta ROLLBACK
  }
  EXPECT_FALSE(usuarioRepo.obtenerPorDni("22222222B").esValido());
}

// 5. Inserción por lotes con fallo en el último elemento (Rollback total)
TEST_F(DatabaseIntegrityTest, RollbackEnCrearMultiplesAnteFalloParcial) {
  int idCine = cineRepo.crear(Cine(-1, "Cine Multi", "Av. Central"));
  int idPeli = peliculaRepo.crear(Pelicula(-1, "Peli Multi", Genero::ACCION, 90));
  int idSala = salaRepo.crear(Sala(-1, idCine, 1, 5, 5));
  Pelicula peli(idPeli, "Peli Multi", Genero::ACCION, 90);
  int idSesion = sesionRepo.crear(Sesion(-1, peli, idSala, 1700000000));

  // Reservamos previamente el asiento (2, 2)
  Reserva ocupada(-1, idSesion, 2, 2, "COMPRADO", 1700000000, "Adulto", 7.5f);
  ASSERT_GT(reservaRepo.crear(ocupada), 0);

  // Intentamos un lote con 4 reservas donde la última colisiona con (2, 2)
  std::vector<Reserva> lote = {
      Reserva(-1, idSesion, 0, 0, "COMPRADO", 1700000000, "Adulto", 7.5f),
      Reserva(-1, idSesion, 0, 1, "COMPRADO", 1700000000, "Niño", 5.0f),
      Reserva(-1, idSesion, 0, 2, "COMPRADO", 1700000000, "Jubilado", 5.5f),
      Reserva(-1, idSesion, 2, 2, "COMPRADO", 1700000000, "Adulto", 7.5f)  // Duplicado
  };

  bool exito = reservaRepo.crearMultiples(lote);
  EXPECT_FALSE(exito);

  // Comprobar que ninguna de las nuevas reservas del lote fue insertada
  auto reservasSesion = reservaRepo.obtenerPorSesion(idSesion);
  EXPECT_EQ(reservasSesion.size(), 1u);
  EXPECT_EQ(reservasSesion[0].getFila(), 2);
  EXPECT_EQ(reservasSesion[0].getColumna(), 2);
}
