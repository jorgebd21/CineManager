#ifndef RESERVAREPOSITORY_HPP
#define RESERVAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/reserva.hpp"

class IReservaRepository {
 public:
  virtual ~IReservaRepository() = default;
  [[nodiscard]] virtual int crear(const Reserva& reserva) = 0;
  [[nodiscard]] virtual bool crearMultiples(const std::vector<Reserva>& reservas) = 0;
  [[nodiscard]] virtual Reserva obtenerPorId(int id) = 0;
  [[nodiscard]] virtual std::vector<Reserva> obtenerPorSesion(int idSesion) = 0;
  [[nodiscard]] virtual std::vector<Reserva> obtenerPendientes() = 0;
  [[nodiscard]] virtual bool actualizar(int id, const Reserva& reserva) = 0;
  [[nodiscard]] virtual bool eliminar(int id) = 0;
};

class ReservaRepository : public IReservaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit ReservaRepository(SqliteDatabase& database);

  [[nodiscard]] int crear(const Reserva& reserva) override;
  [[nodiscard]] bool crearMultiples(const std::vector<Reserva>& reservas) override;
  [[nodiscard]] Reserva obtenerPorId(int id) override;
  [[nodiscard]] std::vector<Reserva> obtenerPorSesion(int idSesion) override;
  [[nodiscard]] std::vector<Reserva> obtenerPendientes() override;
  [[nodiscard]] bool actualizar(int id, const Reserva& reserva) override;
  [[nodiscard]] bool eliminar(int id) override;
};

#endif  // RESERVAREPOSITORY_HPP