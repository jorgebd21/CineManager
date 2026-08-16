#ifndef CINEREPOSITORY_HPP
#define CINEREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/cine.hpp"

class ICineRepository {
 public:
  virtual ~ICineRepository() = default;
  virtual int crear(const Cine& cine) = 0;
  virtual Cine obtenerPorId(int id) = 0;
  virtual std::vector<Cine> obtenerTodos() = 0;
  virtual bool actualizar(int id, const Cine& cine) = 0;
  virtual bool eliminar(int id) = 0;
};

class CineRepository : public ICineRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit CineRepository(SqliteDatabase& database);

  int crear(const Cine& cine) override;
  Cine obtenerPorId(int id) override;
  std::vector<Cine> obtenerTodos() override;
  bool actualizar(int id, const Cine& cine) override;
  bool eliminar(int id) override;
};

#endif  // CINEREPOSITORY_HPP