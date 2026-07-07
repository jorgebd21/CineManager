#ifndef CONSOLA_HPP
#define CONSOLA_HPP

#include "models/sala.hpp"
#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include <vector>

class Consola {
public:
    void mostrarMenu();
    void mostrarSala(const Sala& sala, const std::vector<Reserva>& reservas = {});
    void mostrarPelicula(const Pelicula& pelicula);
};

#endif