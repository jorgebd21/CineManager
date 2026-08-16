#ifndef PELICULAREPOSITORY_HPP
#define PELICULAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/pelicula.hpp"

class IPeliculaRepository {
 public:
  virtual ~IPeliculaRepository() = default;
  virtual int crear(const Pelicula& pelicula) = 0;
  virtual Pelicula obtenerPorId(int id) = 0;
  virtual std::vector<Pelicula> obtenerTodos() = 0;
  virtual std::vector<Pelicula> obtenerCartelera(int idCine) = 0;
  virtual bool actualizar(int id, const Pelicula& pelicula) = 0;
  virtual bool eliminar(int id) = 0;
};

class PeliculaRepository : public IPeliculaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit PeliculaRepository(SqliteDatabase& database);

  int crear(const Pelicula& pelicula) override;
  Pelicula obtenerPorId(int id) override;
  std::vector<Pelicula> obtenerTodos() override;
  std::vector<Pelicula> obtenerCartelera(int idCine) override;
  bool actualizar(int id, const Pelicula& pelicula) override;
  bool eliminar(int id) override;
};

#endif  // PELICULAREPOSITORY_HPP