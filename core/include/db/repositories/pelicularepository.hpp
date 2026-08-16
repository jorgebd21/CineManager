#ifndef PELICULAREPOSITORY_HPP
#define PELICULAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/pelicula.hpp"

class IPeliculaRepository {
 public:
  virtual ~IPeliculaRepository() = default;
  [[nodiscard]] virtual int crear(const Pelicula& pelicula) = 0;
  [[nodiscard]] virtual Pelicula obtenerPorId(int id) = 0;
  [[nodiscard]] virtual std::vector<Pelicula> obtenerTodos() = 0;
  [[nodiscard]] virtual std::vector<Pelicula> obtenerCartelera(int idCine) = 0;
  [[nodiscard]] virtual bool actualizar(int id, const Pelicula& pelicula) = 0;
  [[nodiscard]] virtual bool eliminar(int id) = 0;
};

class PeliculaRepository : public IPeliculaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit PeliculaRepository(SqliteDatabase& database);

  [[nodiscard]] int crear(const Pelicula& pelicula) override;
  [[nodiscard]] Pelicula obtenerPorId(int id) override;
  [[nodiscard]] std::vector<Pelicula> obtenerTodos() override;
  [[nodiscard]] std::vector<Pelicula> obtenerCartelera(int idCine) override;
  [[nodiscard]] bool actualizar(int id, const Pelicula& pelicula) override;
  [[nodiscard]] bool eliminar(int id) override;
};

#endif  // PELICULAREPOSITORY_HPP