#ifndef SESIONREPOSITORY_HPP
#define SESIONREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/sesion.hpp"

class ISesionRepository {
 public:
  virtual ~ISesionRepository() = default;
  [[nodiscard]] virtual int crear(const Sesion& sesion) = 0;
  [[nodiscard]] virtual Sesion obtenerPorId(int id) = 0;
  [[nodiscard]] virtual std::vector<Sesion> obtenerTodos() = 0;
  [[nodiscard]] virtual std::vector<Sesion> obtenerSesionesDeCine(int idCine) = 0;
  [[nodiscard]] virtual std::vector<Sesion> obtenerSesionesDePelicula(int idCine, int idPelicula) = 0;
  [[nodiscard]] virtual bool actualizar(int id, const Sesion& sesion) = 0;
  [[nodiscard]] virtual bool eliminar(int id) = 0;
};

class SesionRepository : public ISesionRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit SesionRepository(SqliteDatabase& database);

  [[nodiscard]] int crear(const Sesion& sesion) override;
  [[nodiscard]] Sesion obtenerPorId(int id) override;
  [[nodiscard]] std::vector<Sesion> obtenerTodos() override;
  [[nodiscard]] std::vector<Sesion> obtenerSesionesDeCine(int idCine) override;
  [[nodiscard]] std::vector<Sesion> obtenerSesionesDePelicula(int idCine, int idPelicula) override;
  [[nodiscard]] bool actualizar(int id, const Sesion& sesion) override;
  [[nodiscard]] bool eliminar(int id) override;
};

#endif  // SESIONREPOSITORY_HPP