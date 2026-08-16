#include <gtest/gtest.h>

#include "db/database.hpp"
#include "db/repositories/reservarepository.hpp"
#include "models/reserva.hpp"

class ReservaRepoTest : public ::testing::Test {
 protected:
  SqliteDatabase db;
  ReservaRepository repo{db};

  void SetUp() override {
    SqliteStatement stmt(db.getDb(),
                         "CREATE TABLE IF NOT EXISTS reservas ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "sesion_id INTEGER NOT NULL, "
                         "fila INTEGER NOT NULL, "
                         "columna INTEGER NOT NULL, "
                         "estado TEXT NOT NULL DEFAULT 'PENDIENTE', "
                         "timestamp_creacion INTEGER NOT NULL DEFAULT 0, "
                         "tipo TEXT, "
                         "precio REAL, "
                         "UNIQUE(sesion_id, fila, columna));");
    stmt.step();

    SqliteStatement clearStmt(db.getDb(), "DELETE FROM reservas;");
    clearStmt.step();
  }
};

TEST_F(ReservaRepoTest, CrearReservaConTarifasDinámicas) {
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
  EXPECT_EQ(rec1.getTipo(), "Adulto");
  EXPECT_FLOAT_EQ(rec1.getPrecio(), 7.50f);

  Reserva rec2 = repo.obtenerPorId(id2);
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
