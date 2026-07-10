#ifndef PELICULAREPOSITORY_HPP
#define PELICULAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/pelicula.hpp"

class PeliculaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit PeliculaRepository(SqliteDatabase& database);

  bool crear(const Pelicula& pelicula);
  Pelicula obtenerPorId(int id);
  std::vector<Pelicula> obtenerTodos();
  std::vector<Pelicula> obtenerCartelera(int idCine);
  bool actualizar(int id, const Pelicula& pelicula);
  bool eliminar(int id);
};

#endif  // PELICULAREPOSITORY_HPP