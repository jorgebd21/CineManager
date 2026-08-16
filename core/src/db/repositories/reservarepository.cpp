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
        !stmt.bindInt64(5, static_cast<sqlite3_int64>(reserva.getTimestampCreacion())) ||
        !stmt.bindText(6, reserva.getTipo()) ||
        !stmt.bindFloat(7, reserva.getPrecio()))
      return -1;

    if (stmt.step() != SQLITE_DONE) return -1;

    return static_cast<int>(sqlite3_last_insert_rowid(db.getDb()));
  } catch (const std::exception& e) {
    std::cerr << "Error en ReservaRepository::crear: " << e.what() << std::endl;
    return -1;
  }
}

bool ReservaRepository::crearMultiples(const std::vector<Reserva>& reservas) {
  if (reservas.empty()) return true;

  SqliteTransaction tx(db.getDb());
  try {
    SqliteStatement stmt(db.getDb(),
                         "INSERT INTO reservas (sesion_id, fila, columna, "
                         "estado, timestamp_creacion, tipo, precio) VALUES (?, "
                         "?, ?, ?, ?, ?, ?)");
    for (const auto& reserva : reservas) {
      stmt.reset();
      if (!stmt.bindInt(1, reserva.getIdSesion()) ||
          !stmt.bindInt(2, reserva.getFila()) ||
          !stmt.bindInt(3, reserva.getColumna()) ||
          !stmt.bindText(4, reserva.getEstado()) ||
          !stmt.bindInt64(5, static_cast<sqlite3_int64>(reserva.getTimestampCreacion())) ||
          !stmt.bindText(6, reserva.getTipo()) ||
          !stmt.bindFloat(7, reserva.getPrecio())) {
        tx.rollback();
        return false;
      }
      if (stmt.step() != SQLITE_DONE) {
        tx.rollback();
        return false;
      }
    }
    return tx.commit();
  } catch (const std::exception& e) {
    std::cerr << "Error en ReservaRepository::crearMultiples: " << e.what() << std::endl;
    tx.rollback();
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
                     static_cast<std::time_t>(stmt.getColumnInt64(5)),
                     stmt.getColumnText(6), stmt.getColumnFloat(7));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en ReservaRepository::obtenerPorId: " << e.what() << std::endl;
  }
  return Reserva(-1, -1, -1, -1, "", 0);
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
      reservas.emplace_back(
          stmt.getColumnInt(0), stmt.getColumnInt(1), stmt.getColumnInt(2),
          stmt.getColumnInt(3), stmt.getColumnText(4),
          static_cast<std::time_t>(stmt.getColumnInt64(5)),
          stmt.getColumnText(6), stmt.getColumnFloat(7));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en ReservaRepository::obtenerPorSesion: " << e.what() << std::endl;
  }

  return reservas;
}

std::vector<Reserva> ReservaRepository::obtenerPendientes() {
  std::vector<Reserva> reservas;

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT id, sesion_id, fila, columna, estado, "
                         "timestamp_creacion, tipo, precio FROM reservas "
                         "WHERE estado = 'PENDIENTE'");

    while (stmt.step() == SQLITE_ROW) {
      reservas.emplace_back(
          stmt.getColumnInt(0), stmt.getColumnInt(1), stmt.getColumnInt(2),
          stmt.getColumnInt(3), stmt.getColumnText(4),
          static_cast<std::time_t>(stmt.getColumnInt64(5)),
          stmt.getColumnText(6), stmt.getColumnFloat(7));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en ReservaRepository::obtenerPendientes: " << e.what() << std::endl;
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

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en ReservaRepository::actualizar: " << e.what() << std::endl;
  }
  return false;
}

bool ReservaRepository::eliminar(int id) {
  try {
    SqliteStatement stmt(db.getDb(), "DELETE FROM reservas WHERE id = ?");
    if (!stmt.bindInt(1, id)) return false;

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en ReservaRepository::eliminar: " << e.what() << std::endl;
  }
  return false;
}