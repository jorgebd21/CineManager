#ifndef CONSOLA_HPP
#define CONSOLA_HPP

#include <vector>

#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include "models/sala.hpp"

class Consola {
 public:
  void mostrarMenu();
  void mostrarSala(const Sala& sala, const std::vector<Reserva>& reservas = {});
  void mostrarPelicula(const Pelicula& pelicula);
  void mostrarMenuAdmin();
  void mostrarSubmenuCines();
  void mostrarSubmenuPeliculas();
  void mostrarSubmenuSalas();
  void mostrarSubmenuSesiones();
};

#endif