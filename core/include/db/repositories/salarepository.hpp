#ifndef SALAREPOSITORY_HPP
#define SALAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/sala.hpp"

class ISalaRepository {
 public:
  virtual ~ISalaRepository() = default;
  [[nodiscard]] virtual int crear(const Sala& sala) = 0;
  [[nodiscard]] virtual Sala obtenerPorId(int id) = 0;
  [[nodiscard]] virtual std::vector<Sala> obtenerTodos() = 0;
  [[nodiscard]] virtual std::vector<Sala> obtenerSalasDeCine(int idCine) = 0;
  [[nodiscard]] virtual bool actualizar(int id, const Sala& sala) = 0;
  [[nodiscard]] virtual bool eliminar(int id) = 0;
};

class SalaRepository : public ISalaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit SalaRepository(SqliteDatabase& database);

  [[nodiscard]] int crear(const Sala& sala) override;
  [[nodiscard]] Sala obtenerPorId(int id) override;
  [[nodiscard]] std::vector<Sala> obtenerTodos() override;
  [[nodiscard]] std::vector<Sala> obtenerSalasDeCine(int idCine) override;
  [[nodiscard]] bool actualizar(int id, const Sala& sala) override;
  [[nodiscard]] bool eliminar(int id) override;
};

#endif  // SALAREPOSITORY_HPP