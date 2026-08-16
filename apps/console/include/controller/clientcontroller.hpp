#ifndef CLIENT_CONTROLLER_HPP
#define CLIENT_CONTROLLER_HPP

#include <iostream>

#include "UI/consola.hpp"
#include "db/datamanager.hpp"
#include "models/usuario.hpp"

class ClientController {
 private:
  DataManager& db;
  Consola& consola;
  Usuario usuarioActual;

 public:
  ClientController(DataManager& database, Consola& ui)
      : db(database), consola(ui) {}

  void ejecutar();

 private:
  bool iniciarSesion();
  void cartelera(int cineId);
  int realizarReserva(int cineId);
  bool realizarCompra(int cineId, int reservaId);
};

#endif  // CLIENT_CONTROLLER_HPP