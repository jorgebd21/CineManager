#ifndef RESERVAREPOSITORY_HPP
#define RESERVAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/reserva.hpp"

class IReservaRepository {
 public:
  virtual ~IReservaRepository() = default;
  virtual int crear(const Reserva& reserva) = 0;
  virtual bool crearMultiples(const std::vector<Reserva>& reservas) = 0;
  virtual Reserva obtenerPorId(int id) = 0;
  virtual std::vector<Reserva> obtenerPorSesion(int idSesion) = 0;
  virtual std::vector<Reserva> obtenerPendientes() = 0;
  virtual bool actualizar(int id, const Reserva& reserva) = 0;
  virtual bool eliminar(int id) = 0;
};

class ReservaRepository : public IReservaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit ReservaRepository(SqliteDatabase& database);

  int crear(const Reserva& reserva) override;
  bool crearMultiples(const std::vector<Reserva>& reservas) override;
  Reserva obtenerPorId(int id) override;
  std::vector<Reserva> obtenerPorSesion(int idSesion) override;
  std::vector<Reserva> obtenerPendientes() override;
  bool actualizar(int id, const Reserva& reserva) override;
  bool eliminar(int id) override;
};

#endif  // RESERVAREPOSITORY_HPP