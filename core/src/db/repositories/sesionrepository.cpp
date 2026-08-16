#include "db/repositories/sesionrepository.hpp"

#include <iostream>

#include "models/pelicula.hpp"

SesionRepository::SesionRepository(SqliteDatabase& database) : db(database) {}

int SesionRepository::crear(const Sesion& sesion) {
  try {
    SqliteStatement stmt(
        db.getDb(),
        "INSERT INTO sesiones (pelicula_id, sala_id, fecha_hora, "
        "precio_entrada) VALUES (?, ?, datetime(?, 'unixepoch'), 10.0)");
    if (!stmt.bindInt(1, sesion.getPelicula().getId()) ||
        !stmt.bindInt(2, sesion.getIdSala()) ||
        !stmt.bindInt64(3, static_cast<sqlite3_int64>(sesion.getHoraInicio())))
      return -1;

    if (stmt.step() != SQLITE_DONE) return -1;

    return static_cast<int>(sqlite3_last_insert_rowid(db.getDb()));
  } catch (const std::exception& e) {
    std::cerr << "Error en SesionRepository::crear: " << e.what() << std::endl;
    return -1;
  }
}

Sesion SesionRepository::obtenerPorId(int id) {
  try {
    // Solución N+1: JOIN directo con peliculas para obtener los datos completos en un solo paso
    SqliteStatement stmt(
        db.getDb(),
        "SELECT s.id, s.sala_id, strftime('%s', s.fecha_hora), "
        "p.id, p.titulo, p.genero, p.duracion "
        "FROM sesiones s "
        "JOIN peliculas p ON s.pelicula_id = p.id "
        "WHERE s.id = ?");
    if (!stmt.bindInt(1, id))
      return Sesion(-1, Pelicula(-1, "", Genero::NONE, 0), -1, 0);

    if (stmt.step() == SQLITE_ROW) {
      Pelicula pelicula(stmt.getColumnInt(3), stmt.getColumnText(4),
                        stringToGenero(stmt.getColumnText(5)),
                        stmt.getColumnInt(6));
      return Sesion(id, std::move(pelicula), stmt.getColumnInt(1),
                    static_cast<std::time_t>(stmt.getColumnInt64(2)));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en SesionRepository::obtenerPorId: " << e.what() << std::endl;
  }
  return Sesion(-1, Pelicula(-1, "", Genero::NONE, 0), -1, 0);
}

std::vector<Sesion> SesionRepository::obtenerTodos() {
  std::vector<Sesion> sesiones;

  try {
    // Solución N+1: JOIN directo con peliculas
    SqliteStatement stmt(
        db.getDb(),
        "SELECT s.id, s.sala_id, strftime('%s', s.fecha_hora), "
        "p.id, p.titulo, p.genero, p.duracion "
        "FROM sesiones s "
        "JOIN peliculas p ON s.pelicula_id = p.id");

    while (stmt.step() == SQLITE_ROW) {
      Pelicula pelicula(stmt.getColumnInt(3), stmt.getColumnText(4),
                        stringToGenero(stmt.getColumnText(5)),
                        stmt.getColumnInt(6));
      sesiones.emplace_back(stmt.getColumnInt(0), std::move(pelicula),
                            stmt.getColumnInt(1),
                            static_cast<std::time_t>(stmt.getColumnInt64(2)));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en SesionRepository::obtenerTodos: " << e.what() << std::endl;
  }

  return sesiones;
}

std::vector<Sesion> SesionRepository::obtenerSesionesDeCine(int idCine) {
  std::vector<Sesion> sesiones;

  try {
    // Solución N+1: JOIN directo con peliculas y salas
    SqliteStatement stmt(
        db.getDb(),
        "SELECT s.id, s.sala_id, strftime('%s', s.fecha_hora), "
        "p.id, p.titulo, p.genero, p.duracion "
        "FROM sesiones s "
        "JOIN peliculas p ON s.pelicula_id = p.id "
        "JOIN salas sa ON s.sala_id = sa.id "
        "WHERE sa.cine_id = ?");

    if (!stmt.bindInt(1, idCine)) return sesiones;

    while (stmt.step() == SQLITE_ROW) {
      Pelicula pelicula(stmt.getColumnInt(3), stmt.getColumnText(4),
                        stringToGenero(stmt.getColumnText(5)),
                        stmt.getColumnInt(6));
      sesiones.emplace_back(stmt.getColumnInt(0), std::move(pelicula),
                            stmt.getColumnInt(1),
                            static_cast<std::time_t>(stmt.getColumnInt64(2)));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en SesionRepository::obtenerSesionesDeCine: " << e.what() << std::endl;
  }

  return sesiones;
}

std::vector<Sesion> SesionRepository::obtenerSesionesDePelicula(
    int idCine, int idPelicula) {
  std::vector<Sesion> sesiones;

  try {
    // Solución N+1: JOIN directo
    SqliteStatement stmt(
        db.getDb(),
        "SELECT s.id, s.sala_id, strftime('%s', s.fecha_hora), "
        "p.id, p.titulo, p.genero, p.duracion "
        "FROM sesiones s "
        "JOIN peliculas p ON s.pelicula_id = p.id "
        "JOIN salas sa ON s.sala_id = sa.id "
        "WHERE sa.cine_id = ? AND s.pelicula_id = ? AND s.fecha_hora >= datetime('now')");

    if (!stmt.bindInt(1, idCine) || !stmt.bindInt(2, idPelicula))
      return sesiones;

    while (stmt.step() == SQLITE_ROW) {
      Pelicula pelicula(stmt.getColumnInt(3), stmt.getColumnText(4),
                        stringToGenero(stmt.getColumnText(5)),
                        stmt.getColumnInt(6));
      sesiones.emplace_back(stmt.getColumnInt(0), std::move(pelicula),
                            stmt.getColumnInt(1),
                            static_cast<std::time_t>(stmt.getColumnInt64(2)));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en SesionRepository::obtenerSesionesDePelicula: " << e.what() << std::endl;
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
        !stmt.bindInt64(3, static_cast<sqlite3_int64>(sesion.getHoraInicio())) ||
        !stmt.bindInt(4, id))
      return false;

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en SesionRepository::actualizar: " << e.what() << std::endl;
  }
  return false;
}

bool SesionRepository::eliminar(int id) {
  try {
    SqliteStatement stmt(db.getDb(), "DELETE FROM sesiones WHERE id = ?");
    if (!stmt.bindInt(1, id)) return false;

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en SesionRepository::eliminar: " << e.what() << std::endl;
  }
  return false;
}