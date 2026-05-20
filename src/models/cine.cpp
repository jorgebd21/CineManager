#include "models/cine.hpp"

Cine::Cine() {}

std::vector<Sala>& Cine::getSalas( ){
    return salas;
}

std::vector<Pelicula>& Cine::getPeliculas(){
    return peliculas;
}

std::vector<Sesion>& Cine::getSesiones(){
    return sesiones;
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

bool Cine::agregarSala(Sala& sala) {
    salas.push_back(sala);
    return true;
}

bool Cine::agregarPelicula(const Pelicula& pelicula) {
    peliculas.push_back(pelicula);
    return true;
}

bool Cine::agregarSesion(Sala& sala, const Pelicula& pelicula, std::tm horaInicio) {
    if (sala.estaOcupada()) {
        return false;
    }

    bool esta = false;
    for(auto sala : getSalas()) {
        if (sala.getId() == sala.getId()) {
            esta = true;
            break;
        }
    }
    if (!esta) {
        return false;
    }

    esta = false;
    for(auto pelicula : getPeliculas()) {
        if (pelicula.getId() == pelicula.getId()) {
            esta = true;
            break;
        }
    }
    if (!esta) {
        return false;
    }

    sesiones.push_back(Sesion(pelicula, sala, horaInicio));
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

