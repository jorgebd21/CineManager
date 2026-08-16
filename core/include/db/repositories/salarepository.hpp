#ifndef SALAREPOSITORY_HPP
#define SALAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/sala.hpp"

class ISalaRepository {
 public:
  virtual ~ISalaRepository() = default;
  virtual int crear(const Sala& sala) = 0;
  virtual Sala obtenerPorId(int id) = 0;
  virtual std::vector<Sala> obtenerTodos() = 0;
  virtual std::vector<Sala> obtenerSalasDeCine(int idCine) = 0;
  virtual bool actualizar(int id, const Sala& sala) = 0;
  virtual bool eliminar(int id) = 0;
};

class SalaRepository : public ISalaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit SalaRepository(SqliteDatabase& database);

  int crear(const Sala& sala) override;
  Sala obtenerPorId(int id) override;
  std::vector<Sala> obtenerTodos() override;
  std::vector<Sala> obtenerSalasDeCine(int idCine) override;
  bool actualizar(int id, const Sala& sala) override;
  bool eliminar(int id) override;
};

#endif  // SALAREPOSITORY_HPP