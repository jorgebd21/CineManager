#ifndef SALAREPOSITORY_HPP
#define SALAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/sala.hpp"

class SalaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit SalaRepository(SqliteDatabase& database);

  bool crear(const Sala& sala);
  Sala obtenerPorId(int id);
  std::vector<Sala> obtenerTodos();
  std::vector<Sala> obtenerSalasDeCine(int idCine);
  bool actualizar(int id, const Sala& sala);
  bool eliminar(int id);
};

#endif  // SALAREPOSITORY_HPP