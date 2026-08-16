#include "db/repositories/pelicularepository.hpp"

#include <iostream>

#include "models/pelicula.hpp"

PeliculaRepository::PeliculaRepository(SqliteDatabase& database)
    : db(database) {}

int PeliculaRepository::crear(const Pelicula& pelicula) {
  try {
    SqliteStatement stmt(
        db.getDb(),
        "INSERT INTO peliculas (titulo, genero, duracion) VALUES (?, ?, ?)");
    if (!stmt.bindText(1, pelicula.getTitulo()) ||
        !stmt.bindText(2, generoToString(pelicula.getGenero())) ||
        !stmt.bindInt(3, pelicula.getDuracion()))
      return -1;

    if (stmt.step() != SQLITE_DONE) return -1;

    return static_cast<int>(sqlite3_last_insert_rowid(db.getDb()));
  } catch (const std::exception& e) {
    std::cerr << "Error en PeliculaRepository::crear: " << e.what() << std::endl;
    return -1;
  }
}

Pelicula PeliculaRepository::obtenerPorId(int id) {
  try {
    SqliteStatement stmt(
        db.getDb(),
        "SELECT titulo, genero, duracion FROM peliculas WHERE id = ?");
    if (!stmt.bindInt(1, id)) return Pelicula(-1, "", Genero::NONE, 0);

    if (stmt.step() == SQLITE_ROW) {
      return Pelicula(id, stmt.getColumnText(0),
                      stringToGenero(stmt.getColumnText(1)),
                      stmt.getColumnInt(2));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en PeliculaRepository::obtenerPorId: " << e.what() << std::endl;
  }
  return Pelicula(-1, "", Genero::NONE, 0);
}

std::vector<Pelicula> PeliculaRepository::obtenerTodos() {
  std::vector<Pelicula> peliculas;

  try {
    SqliteStatement stmt(db.getDb(),
                         "SELECT id, titulo, genero, duracion FROM peliculas");

    while (stmt.step() == SQLITE_ROW) {
      peliculas.emplace_back(stmt.getColumnInt(0), stmt.getColumnText(1),
                             stringToGenero(stmt.getColumnText(2)),
                             stmt.getColumnInt(3));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en PeliculaRepository::obtenerTodos: " << e.what() << std::endl;
  }

  return peliculas;
}

std::vector<Pelicula> PeliculaRepository::obtenerCartelera(int idCine) {
  std::vector<Pelicula> peliculas;

  try {
    SqliteStatement stmt(
        db.getDb(),
        "SELECT DISTINCT p.id, p.titulo, p.genero, p.duracion FROM peliculas p "
        "JOIN sesiones s ON p.id = s.pelicula_id JOIN salas sa ON s.sala_id = "
        "sa.id WHERE sa.cine_id = ? AND s.fecha_hora >= datetime('now')");

    if (!stmt.bindInt(1, idCine)) return peliculas;

    while (stmt.step() == SQLITE_ROW) {
      peliculas.emplace_back(stmt.getColumnInt(0), stmt.getColumnText(1),
                             stringToGenero(stmt.getColumnText(2)),
                             stmt.getColumnInt(3));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error en PeliculaRepository::obtenerCartelera: " << e.what() << std::endl;
  }

  return peliculas;
}

bool PeliculaRepository::actualizar(int id, const Pelicula& pelicula) {
  try {
    SqliteStatement stmt(db.getDb(),
                         "UPDATE peliculas SET titulo = ?, genero = ?, "
                         "duracion = ? WHERE id = ?");
    if (!stmt.bindText(1, pelicula.getTitulo()) ||
        !stmt.bindText(2, generoToString(pelicula.getGenero())) ||
        !stmt.bindInt(3, pelicula.getDuracion()) || !stmt.bindInt(4, id))
      return false;

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en PeliculaRepository::actualizar: " << e.what() << std::endl;
  }
  return false;
}

bool PeliculaRepository::eliminar(int id) {
  try {
    SqliteStatement stmt(db.getDb(), "DELETE FROM peliculas WHERE id = ?");
    if (!stmt.bindInt(1, id)) return false;

    return (stmt.step() == SQLITE_DONE);
  } catch (const std::exception& e) {
    std::cerr << "Error en PeliculaRepository::eliminar: " << e.what() << std::endl;
  }
  return false;
}