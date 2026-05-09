#ifndef CINE_H
#define CINE_H

#include "models/sala.hpp"
#include "models/pelicula.hpp"
#include <set>

class Cine{
private:
    std::set<Sala> salas;
    std::set<Pelicula> peliculas;
public:
    Cine();

    std::set<Sala> getSalas() const;
    std::set<Pelicula> getPeliculas() const;

    bool agregarSala(const Sala& sala);
    bool agregarPelicula(const Pelicula& pelicula);

    bool eliminarSala(int id);
    bool eliminarPelicula(int id);
};

#endif