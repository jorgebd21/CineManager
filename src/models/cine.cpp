#include "models/cine.hpp"

Cine::Cine() {}

std::vector<Sala>& Cine::getSalas( ){
    return salas;
}

std::vector<Pelicula>& Cine::getPeliculas(){
    return peliculas;
}

std::vector<Sala> Cine::getSalasDisponibles(){
    std::vector<Sala> salasDisponibles;
    for (auto& sala : salas) {
        if (!sala.estaOcupada()) {
            salasDisponibles.push_back(sala);
        }
    }
    return salasDisponibles;
}

std::vector<Sala> Cine::getSalasOcupadas(){
    std::vector<Sala> salasOcupadas;
    for (auto& sala : salas) {
        if (sala.estaOcupada()) {
            salasOcupadas.push_back(sala);
        }
    }
    return salasOcupadas;
}

bool Cine::agregarSala(const Sala& sala) {
    salas.push_back(sala);
    return true;
}

bool Cine::agregarPelicula(const Pelicula& pelicula) {
    peliculas.push_back(pelicula);
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

