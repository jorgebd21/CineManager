#ifndef SESIONREPOSITORY_HPP
#define SESIONREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/sesion.hpp"

class ISesionRepository {
 public:
  virtual ~ISesionRepository() = default;
  virtual int crear(const Sesion& sesion) = 0;
  virtual Sesion obtenerPorId(int id) = 0;
  virtual std::vector<Sesion> obtenerTodos() = 0;
  virtual std::vector<Sesion> obtenerSesionesDeCine(int idCine) = 0;
  virtual std::vector<Sesion> obtenerSesionesDePelicula(int idCine, int idPelicula) = 0;
  virtual bool actualizar(int id, const Sesion& sesion) = 0;
  virtual bool eliminar(int id) = 0;
};

class SesionRepository : public ISesionRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit SesionRepository(SqliteDatabase& database);

  int crear(const Sesion& sesion) override;
  Sesion obtenerPorId(int id) override;
  std::vector<Sesion> obtenerTodos() override;
  std::vector<Sesion> obtenerSesionesDeCine(int idCine) override;
  std::vector<Sesion> obtenerSesionesDePelicula(int idCine, int idPelicula) override;
  bool actualizar(int id, const Sesion& sesion) override;
  bool eliminar(int id) override;
};

#endif  // SESIONREPOSITORY_HPP