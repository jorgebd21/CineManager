#ifndef ADMIN_CONTROLLER_HPP
#define ADMIN_CONTROLLER_HPP

#include <iostream>

#include "UI/consola.hpp"
#include "db/datamanager.hpp"
#include "models/pelicula.hpp"
#include "models/usuario.hpp"

class AdminController {
 private:
  DataManager& db;
  Consola& consola;
  Usuario usuarioActual;

 public:
  AdminController(DataManager& database, Consola& ui)
      : db(database), consola(ui) {}

  void ejecutar();

 private:
  bool iniciarSesion();
  void gestionarCines();
  void gestionarPeliculas();
  void gestionarSalas();
  void gestionarSesiones();
  void mostrarReporteOcupacion();
};

#endif  // ADMIN_CONTROLLER_HPP