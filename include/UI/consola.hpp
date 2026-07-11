#ifndef CONSOLA_HPP
#define CONSOLA_HPP

#include <vector>

#include "models/cine.hpp"
#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include "models/sala.hpp"
#include "models/sesion.hpp"

class Consola {
 public:
  void mostrarMenu();
  void mostrarSala(const Sala& sala, const std::vector<Reserva>& reservas = {});
  void mostrarPelicula(const Pelicula& pelicula);
  void mostrarCine(const Cine& cine);
  void mostrarSesion(const Sesion& sesion);
  void mostrarReporte(int totales, int ocupados, std::string titulo = "");
  void mostrarTicket(const Reserva& reserva, const Pelicula& pelicula,
                     const Sesion& sesion);

  void mostrarMenuAdmin();
  void mostrarSubmenuCines();
  void mostrarSubmenuPeliculas();
  void mostrarSubmenuSalas();
  void mostrarSubmenuSesiones();

  int pedirEntero(const std::string& mensaje);
  std::string pedirCadena(const std::string& mensaje);
};

#endif