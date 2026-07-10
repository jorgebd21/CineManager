#ifndef CINEREPOSITORY_HPP
#define CINEREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/cine.hpp"

class CineRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit CineRepository(SqliteDatabase& database);

  bool crear(const Cine& cine);
  Cine obtenerPorId(int id);
  std::vector<Cine> obtenerTodos();
  bool actualizar(int id, const Cine& cine);
  bool eliminar(int id);
};

#endif  // CINEREPOSITORY_HPP