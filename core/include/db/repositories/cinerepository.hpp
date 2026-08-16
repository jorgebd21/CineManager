#ifndef CINEREPOSITORY_HPP
#define CINEREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/cine.hpp"

class ICineRepository {
 public:
  virtual ~ICineRepository() = default;
  [[nodiscard]] virtual int crear(const Cine& cine) = 0;
  [[nodiscard]] virtual Cine obtenerPorId(int id) = 0;
  [[nodiscard]] virtual std::vector<Cine> obtenerTodos() = 0;
  [[nodiscard]] virtual bool actualizar(int id, const Cine& cine) = 0;
  [[nodiscard]] virtual bool eliminar(int id) = 0;
};

class CineRepository : public ICineRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit CineRepository(SqliteDatabase& database);

  [[nodiscard]] int crear(const Cine& cine) override;
  [[nodiscard]] Cine obtenerPorId(int id) override;
  [[nodiscard]] std::vector<Cine> obtenerTodos() override;
  [[nodiscard]] bool actualizar(int id, const Cine& cine) override;
  [[nodiscard]] bool eliminar(int id) override;
};

#endif  // CINEREPOSITORY_HPP