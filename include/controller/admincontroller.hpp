#ifndef ADMIN_CONTROLLER_HPP
#define ADMIN_CONTROLLER_HPP

#include "UI/consola.hpp"
#include "db/datamanager.hpp"

class AdminController {
 private:
  DataManager& db;
  Consola& consola;

 public:
  AdminController(DataManager& database, Consola& ui);

  void ejecutar();

 private:
  void gestionarCines();
  void gestionarPeliculas();
  void gestionarSalas();
  void gestionarSesiones();
  void mostrarReporteOcupacion();
};

#endif  // ADMIN_CONTROLLER_HPP