#ifndef ADMIN_CONTROLLER_HPP
#define ADMIN_CONTROLLER_HPP

#include <endian.h>

#include <iostream>

#include "UI/consola.hpp"
#include "db/datamanager.hpp"
#include "models/pelicula.hpp"

class AdminController {
 private:
  DataManager& db;
  Consola& consola;

 public:
  AdminController(DataManager& database, Consola& ui)
      : db(database), consola(ui) {}

  void ejecutar();

 private:
  void gestionarCines();
  void gestionarPeliculas();
  void gestionarSalas();
  void gestionarSesiones();
  void mostrarReporteOcupacion();
};

#endif  // ADMIN_CONTROLLER_HPP