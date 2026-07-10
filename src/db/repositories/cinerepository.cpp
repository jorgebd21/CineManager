#include "db/repositories/cinerepository.hpp"

#include <iostream>

CineRepository::CineRepository(SqliteDatabase& database) : db(database) {}

bool CineRepository::crear(const Cine& cine) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "INSERT INTO cines (nombre, direccion) VALUES (?, ?)");
    if (!stmt.bindText(1, cine.getNombre()) ||
        !stmt.bindText(2, cine.getDireccion()))
      return false;
    return stmt.step() == SQLITE_DONE;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

Cine CineRepository::obtenerPorId(int id) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT nombre, direccion FROM cines WHERE id = ?");
    if (!stmt.bindInt(1, id)) return Cine(-1, "", "");

    if (stmt.step() == SQLITE_ROW) {
      return Cine(id, stmt.getColumnText(0), stmt.getColumnText(1));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return Cine(-1, "", "");
}

std::vector<Cine> CineRepository::obtenerTodos() {
  std::vector<Cine> cines;

  try {
    SqliteStatement stmt(db.getDb(), "SELECT id, nombre, direccion FROM cines");

    while (stmt.step() == SQLITE_ROW) {
      cines.push_back(Cine(stmt.getColumnInt(0), stmt.getColumnText(1),
                           stmt.getColumnText(2)));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return cines;
}

bool CineRepository::actualizar(int id, const Cine& cine) {
  try {
    SqliteStatement stmt(
        db.getDb(), "UPDATE cines SET nombre = ?, direccion = ? WHERE id = ?");
    if (!stmt.bindText(1, cine.getNombre()) ||
        !stmt.bindText(2, cine.getDireccion()) || !stmt.bindInt(3, id))
      return false;

    if (stmt.step() == SQLITE_DONE) {
      return true;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}

bool CineRepository::eliminar(int id) {
  try {
    SqliteStatement stmt(db.getDb(), "DELETE FROM cines WHERE id = ?");
    if (!stmt.bindInt(1, id)) return false;

    if (stmt.step() == SQLITE_DONE) {
      return true;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}