#include "db/repositories/sesionrepository.hpp"

#include <iostream>

#include "db/repositories/pelicularepository.hpp"

SesionRepository::SesionRepository(SqliteDatabase& database) : db(database) {}

bool SesionRepository::crear(const Sesion& sesion) {
  try {
    SqliteStatement stmt(
        db.getDb(),
        "INSERT INTO sesiones (pelicula_id, sala_id, fecha_hora, "
        "precio_entrada) VALUES (?, ?, datetime(?, 'unixepoch'), 10.0)");
    if (!stmt.bindInt(1, sesion.getPelicula().getId()) ||
        !stmt.bindInt(2, sesion.getIdSala()) ||
        !stmt.bindInt(3, sesion.getHoraInicio()))
      return false;
    return stmt.step() == SQLITE_DONE;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

Sesion SesionRepository::obtenerPorId(int id) {
  PeliculaRepository peliculaRepo(db);

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT pelicula_id, sala_id, strftime('%s', "
                         "fecha_hora) FROM sesiones WHERE id = ?");
    if (!stmt.bindInt(1, id))
      return Sesion(-1, Pelicula(-1, "", Genero::NONE, 0), -1, 0);

    if (stmt.step() == SQLITE_ROW) {
      return Sesion(id, peliculaRepo.obtenerPorId(stmt.getColumnInt(0)),
                    stmt.getColumnInt(1), stmt.getColumnInt(2));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return Sesion(-1, Pelicula(-1, "", Genero::NONE, 0), -1, 0);
}

std::vector<Sesion> SesionRepository::obtenerTodos() {
  std::vector<Sesion> sesiones;
  PeliculaRepository peliculaRepo(db);

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT id, pelicula_id, sala_id, strftime('%s', "
                         "fecha_hora) FROM sesiones");

    while (stmt.step() == SQLITE_ROW) {
      sesiones.push_back(Sesion(stmt.getColumnInt(0),
                                peliculaRepo.obtenerPorId(stmt.getColumnInt(1)),
                                stmt.getColumnInt(2), stmt.getColumnInt(3)));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return sesiones;
}

std::vector<Sesion> SesionRepository::obtenerSesionesDeCine(int idCine) {
  std::vector<Sesion> sesiones;
  PeliculaRepository peliculaRepo(db);

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT s.id, s.pelicula_id, s.sala_id, "
                         "strftime('%s', s.fecha_hora) FROM sesiones s JOIN "
                         "salas sa ON s.sala_id = sa.id WHERE sa.cine_id = ?");

    if (!stmt.bindInt(1, idCine)) return sesiones;

    while (stmt.step() == SQLITE_ROW) {
      sesiones.push_back(Sesion(stmt.getColumnInt(0),
                                peliculaRepo.obtenerPorId(stmt.getColumnInt(1)),
                                stmt.getColumnInt(2), stmt.getColumnInt(3)));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return sesiones;
}

std::vector<Sesion> SesionRepository::obtenerSesionesDePelicula(
    int idCine, int idPelicula) {
  std::vector<Sesion> sesiones;
  PeliculaRepository peliculaRepo(db);

  try {
    SqliteStatement stmt(
        db.getDb(),
        "SELECT s.id, s.pelicula_id, s.sala_id, strftime('%s', s.fecha_hora) "
        "FROM sesiones s JOIN salas sa ON s.sala_id = sa.id WHERE sa.cine_id = "
        "? AND s.pelicula_id = ?");

    if (!stmt.bindInt(1, idCine) || !stmt.bindInt(2, idPelicula))
      return sesiones;

    while (stmt.step() == SQLITE_ROW) {
      sesiones.push_back(Sesion(stmt.getColumnInt(0),
                                peliculaRepo.obtenerPorId(stmt.getColumnInt(1)),
                                stmt.getColumnInt(2), stmt.getColumnInt(3)));
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return sesiones;
}

bool SesionRepository::actualizar(int id, const Sesion& sesion) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "UPDATE sesiones SET pelicula_id = ?, sala_id = ?, "
                         "fecha_hora = datetime(?, 'unixepoch') WHERE id = ?");
    if (!stmt.bindInt(1, sesion.getPelicula().getId()) ||
        !stmt.bindInt(2, sesion.getIdSala()) ||
        !stmt.bindInt(3, sesion.getHoraInicio()) || !stmt.bindInt(4, id))
      return false;

    if (stmt.step() == SQLITE_DONE) {
      return true;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}

bool SesionRepository::eliminar(int id) {
  try {
    SqliteStatement stmt(db.getDb(), "DELETE FROM sesiones WHERE id = ?");
    if (!stmt.bindInt(1, id)) return false;

    if (stmt.step() == SQLITE_DONE) {
      return true;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return false;
}