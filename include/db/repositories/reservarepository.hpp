#ifndef RESERVAREPOSITORY_HPP
#define RESERVAREPOSITORY_HPP

#include <vector>

#include "db/database.hpp"
#include "models/reserva.hpp"

class ReservaRepository {
 private:
  SqliteDatabase& db;

 public:
  explicit ReservaRepository(SqliteDatabase& database);

  bool crear(const Reserva& reserva);
  Reserva obtenerPorId(int id);
  std::vector<Reserva> obtenerPorSesion(int idSesion);
  bool actualizar(int id, const Reserva& reserva);
  bool eliminar(int id);
};

#endif  // RESERVAREPOSITORY_HPP