#ifndef CLIENT_CONTROLLER_HPP
#define CLIENT_CONTROLLER_HPP

#include <iostream>

#include "UI/consola.hpp"
#include "db/datamanager.hpp"

class ClientController {
 private:
  DataManager& db;
  Consola& consola;

  Reserva reserva;

 public:
  ClientController(DataManager& database, Consola& ui)
      : db(database), consola(ui), reserva(-1, -1, -1, -1) {}

  void ejecutar();

 private:
  void cartelera(int cineId);
  Reserva realizarReserva(int cineId);
  bool realizarCompra(int cineId, const Reserva& reserva);
};

#endif  // CLIENT_CONTROLLER_HPP