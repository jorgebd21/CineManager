#ifndef CONSOLA_HPP
#define CONSOLA_HPP

#include "models/sala.hpp"

class Consola {
public:
    void mostrarMenu();
    void mostrarSala(const Sala& sala);
    void mostrarPelicula(const Pelicula& pelicula);
};

#endif