#include "db/repositories/salarepository.hpp"

#include <iostream>

#include "models/sala.hpp"

SalaRepository::SalaRepository(SqliteDatabase& database) : db(database) {}

bool SalaRepository::crear(const Sala& sala) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "INSERT INTO salas (cine_id, numero_sala, filas, "
                         "columnas) VALUES (?, ?, "
                         "?, ?)");
    if (!stmt.bindInt(1, sala.getCineId()) ||
        !stmt.bindInt(2, sala.getNumeroSala()) ||
        !stmt.bindInt(3, sala.getFilas()) ||
        !stmt.bindInt(4, sala.getColumnas()))
      return false;
    return stmt.step() == SQLITE_DONE;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

Sala SalaRepository::obtenerPorId(int id) {
  try {
    SqliteStatement stmt(
        db.getDb(),
        "SELECT cine_id, numero_sala, filas, columnas FROM salas WHERE id = ?");
    if (!stmt.bindInt(1, id)) return Sala(-1, -1, -1, -1, -1);

    if (stmt.step() == SQLITE_ROW) {
      return Sala(id, stmt.getColumnInt(0), stmt.getColumnInt(1),
                  stmt.getColumnInt(2), stmt.getColumnInt(3));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return Sala(-1, -1, -1, -1, -1);
}

std::vector<Sala> SalaRepository::obtenerTodos() {
  std::vector<Sala> salas;

  try {
    SqliteStatement stmt(
        db.getDb(),
        "SELECT id, cine_id, numero_sala, filas, columnas FROM salas");

    while (stmt.step() == SQLITE_ROW) {
      salas.push_back(Sala(stmt.getColumnInt(0), stmt.getColumnInt(1),
                           stmt.getColumnInt(2), stmt.getColumnInt(3),
                           stmt.getColumnInt(4)));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return salas;
}

std::vector<Sala> SalaRepository::obtenerSalasDeCine(int idCine) {
  std::vector<Sala> salas;

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT id, cine_id, numero_sala, filas, columnas "
                         "FROM salas WHERE cine_id = ?");

    if (!stmt.bindInt(1, idCine)) return salas;

    while (stmt.step() == SQLITE_ROW) {
      salas.push_back(Sala(stmt.getColumnInt(0), stmt.getColumnInt(1),
                           stmt.getColumnInt(2), stmt.getColumnInt(3),
                           stmt.getColumnInt(4)));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return salas;
}

bool SalaRepository::actualizar(int id, const Sala& sala) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "UPDATE salas SET cine_id = ?, numero_sala = ?, filas "
                         "= ?, columnas = ? WHERE id = ?");
    if (!stmt.bindInt(1, sala.getCineId()) ||
        !stmt.bindInt(2, sala.getNumeroSala()) ||
        !stmt.bindInt(3, sala.getFilas()) ||
        !stmt.bindInt(4, sala.getColumnas()) || !stmt.bindInt(5, id))
      return false;

    if (stmt.step() == SQLITE_DONE) {
      return true;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}

bool SalaRepository::eliminar(int id) {
  try {
    SqliteStatement stmt(db.getDb(), "DELETE FROM salas WHERE id = ?");
    if (!stmt.bindInt(1, id)) return false;

    if (stmt.step() == SQLITE_DONE) {
      return true;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}