#include "db/repositories/reservarepository.hpp"

#include <iostream>

ReservaRepository::ReservaRepository(SqliteDatabase& database) : db(database) {}

bool ReservaRepository::crear(const Reserva& reserva) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "INSERT INTO reservas (sesion_id, fila, columna, "
                         "estado) VALUES (?, ?, ?, 'COMPRADO')");
    if (!stmt.bindInt(1, reserva.getIdSesion()) ||
        !stmt.bindInt(2, reserva.getFila()) ||
        !stmt.bindInt(3, reserva.getColumna()))
      return false;
    return stmt.step() == SQLITE_DONE;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

Reserva ReservaRepository::obtenerPorId(int id) {
  try {
    SqliteStatement stmt(
        db.getDb(),
        "SELECT id, sesion_id, fila, columna FROM reservas WHERE id = ?");

    if (!stmt.bindInt(1, id)) return Reserva(-1, -1, -1, -1);

    if (stmt.step() == SQLITE_ROW) {
      return Reserva(id, stmt.getColumnInt(1), stmt.getColumnInt(2),
                     stmt.getColumnInt(3));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return Reserva(-1, -1, -1, -1);
}

std::vector<Reserva> ReservaRepository::obtenerPorSesion(int idSesion) {
  std::vector<Reserva> reservas;

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT id, sesion_id, fila, columna FROM reservas "
                         "WHERE sesion_id = ?");

    if (!stmt.bindInt(1, idSesion)) return reservas;

    while (stmt.step() == SQLITE_ROW) {
      reservas.push_back(Reserva(stmt.getColumnInt(0), stmt.getColumnInt(1),
                                 stmt.getColumnInt(2), stmt.getColumnInt(3)));
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
                         "= ? WHERE id = ?");
    if (!stmt.bindInt(1, reserva.getIdSesion()) ||
        !stmt.bindInt(2, reserva.getFila()) ||
        !stmt.bindInt(3, reserva.getColumna()) || !stmt.bindInt(4, id))
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