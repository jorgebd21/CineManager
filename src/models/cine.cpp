#include "models/cine.hpp"

Cine::Cine() {
    salas = std::set<Sala>();
    peliculas = std::set<Pelicula>();
}

std::set<Sala> Cine::getSalas() const {
    return salas;
}

std::set<Pelicula> Cine::getPeliculas() const {
    return peliculas;
}

bool Cine::agregarSala(const Sala& sala) {
    salas.insert(sala);
    return true;
}

bool Cine::agregarPelicula(const Pelicula& pelicula) {
    peliculas.insert(pelicula);
    return true;
}

bool Cine::eliminarSala(int id) {
    for (auto it = salas.begin(); it != salas.end(); ++it) {
        if (it->getId() == id) {
            salas.erase(it);
            return true;
        }
    }
    return false;
}

bool Cine::eliminarPelicula(int id) {
    for (auto it = peliculas.begin(); it != peliculas.end(); ++it) {
        if (it->getId() == id) {
            peliculas.erase(it);
            return true;
        }
    }
    return false;
}

