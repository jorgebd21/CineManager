#ifndef SESIONREPOSITORY_HPP
#define SESIONREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/sesion.hpp"

class SesionRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit SesionRepository(SqliteDatabase& database);

  int crear(const Sesion& sesion);
  Sesion obtenerPorId(int id);
  std::vector<Sesion> obtenerTodos();
  std::vector<Sesion> obtenerSesionesDeCine(int idCine);
  std::vector<Sesion> obtenerSesionesDePelicula(int idCine, int idPelicula);
  bool actualizar(int id, const Sesion& sesion);
  bool eliminar(int id);
};

#endif  // SESIONREPOSITORY_HPP