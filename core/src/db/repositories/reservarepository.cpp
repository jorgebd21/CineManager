#include "db/repositories/reservarepository.hpp"

#include <iostream>

ReservaRepository::ReservaRepository(SqliteDatabase& database) : db(database) {}

int ReservaRepository::crear(const Reserva& reserva) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "INSERT INTO reservas (sesion_id, fila, columna, "
                         "estado, timestamp_creacion, tipo, precio) VALUES (?, "
                         "?, ?, ?, ?, ?, ?)");
    if (!stmt.bindInt(1, reserva.getIdSesion()) ||
        !stmt.bindInt(2, reserva.getFila()) ||
        !stmt.bindInt(3, reserva.getColumna()) ||
        !stmt.bindText(4, reserva.getEstado()) ||
        !stmt.bindInt(5, reserva.getTimestampCreacion()) ||
        !stmt.bindText(6, reserva.getTipo()) ||
        !stmt.bindFloat(7, reserva.getPrecio()))
      return false;
    if (stmt.step() != SQLITE_DONE) return -1;

    return sqlite3_last_insert_rowid(db.getDb());
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

Reserva ReservaRepository::obtenerPorId(int id) {
  try {
    SqliteStatement stmt(
        db.getDb(),
        "SELECT id, sesion_id, fila, columna, estado, "
        "timestamp_creacion, tipo, precio FROM reservas WHERE id = ?");

    if (!stmt.bindInt(1, id)) return Reserva(-1, -1, -1, -1);

    if (stmt.step() == SQLITE_ROW) {
      return Reserva(id, stmt.getColumnInt(1), stmt.getColumnInt(2),
                     stmt.getColumnInt(3), stmt.getColumnText(4),
                     stmt.getColumnInt(5), stmt.getColumnText(6),
                     stmt.getColumnFloat(7));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return Reserva(-1, -1, -1, -1, "", -1);
}

std::vector<Reserva> ReservaRepository::obtenerPorSesion(int idSesion) {
  std::vector<Reserva> reservas;

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT id, sesion_id, fila, columna, estado, "
                         "timestamp_creacion, tipo, precio FROM reservas "
                         "WHERE sesion_id = ?");

    if (!stmt.bindInt(1, idSesion)) return reservas;

    while (stmt.step() == SQLITE_ROW) {
      reservas.push_back(Reserva(
          stmt.getColumnInt(0), stmt.getColumnInt(1), stmt.getColumnInt(2),
          stmt.getColumnInt(3), stmt.getColumnText(4), stmt.getColumnInt(5),
          stmt.getColumnText(6), stmt.getColumnFloat(7)));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return reservas;
}

std::vector<Reserva> ReservaRepository::obtenerPendientes() {
  std::vector<Reserva> reservas;

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT id, sesion_id, timestamp_creacion FROM "
                         "reservas WHERE estado = 'PENDIENTE'");

    while (stmt.step() == SQLITE_ROW) {
      reservas.push_back(Reserva(stmt.getColumnInt(0), stmt.getColumnInt(1), -1,
                                 -1, "ANALIZANDO", stmt.getColumnInt(2)));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return reservas;
}

bool ReservaRepository::actualizar(int id, const Reserva& reserva) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "UPDATE reservas SET sesion_id = ?, fila = ?, columna "
                         "= ?, estado = ?, tipo = ?, precio = ? WHERE id = ?");
    if (!stmt.bindInt(1, reserva.getIdSesion()) ||
        !stmt.bindInt(2, reserva.getFila()) ||
        !stmt.bindInt(3, reserva.getColumna()) ||
        !stmt.bindText(4, reserva.getEstado()) ||
        !stmt.bindText(5, reserva.getTipo()) ||
        !stmt.bindFloat(6, reserva.getPrecio()) || !stmt.bindInt(7, id))
      return false;

    if (stmt.step() == SQLITE_DONE) {
      return true;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}

bool ReservaRepository::eliminar(int id) {
  try {
    SqliteStatement stmt(db.getDb(), "DELETE FROM reservas WHERE id = ?");
    if (!stmt.bindInt(1, id)) return false;

    if (stmt.step() == SQLITE_DONE) {
      return true;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}