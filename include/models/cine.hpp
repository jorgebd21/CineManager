#ifndef CINE_H
#define CINE_H

#include "models/sala.hpp"
#include "models/pelicula.hpp"
#include <vector>

class Cine{
private:
    std::vector<Sala> salas;
    std::vector<Pelicula> peliculas;
public:
    Cine();

    std::vector<Sala> getSalas() const;
    std::vector<Pelicula> getPeliculas() const;

    bool agregarSala(const Sala& sala);
    bool agregarPelicula(const Pelicula& pelicula);

    bool eliminarSala(int id);
    bool eliminarPelicula(int id);
};

#endif