#ifndef CINE_H
#define CINE_H

#include "models/sala.hpp"
#include "models/pelicula.hpp"
#include "models/sesion.hpp"
#include <vector>

class Cine{
private:
    std::vector<Sala> salas;
    std::vector<Pelicula> peliculas;
    std::vector<Sesion> sesiones;
public:
    Cine();

    std::vector<Sala>& getSalas();
    std::vector<Pelicula>& getPeliculas();
    std::vector<Sesion>& getSesiones();

    std::vector<Sala> getSalasDisponibles();

    bool agregarSala(Sala& sala);
    bool agregarPelicula(const Pelicula& pelicula);
    bool agregarSesion(Sala& sala, const Pelicula& pelicula, std::tm horaInicio);

    bool eliminarSala(int id);
    bool eliminarPelicula(int id);
};

#endif