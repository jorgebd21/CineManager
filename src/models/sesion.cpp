#include "models/sesion.hpp"
#include "models/pelicula.hpp"
#include "models/sala.hpp"

int Sesion::contador = 0;

void Sesion::calcularHoraFin(){
    horaFin.tm_min = horaInicio.tm_min + pelicula->getDuracion();
}

Sesion::Sesion(const Pelicula &pelicula, Sala &sala, std::tm horaInicio) {
    if(&pelicula == nullptr || &sala == nullptr || !sala.reservarSala(pelicula)) {
        id = -1;
    }else{
        id = ++contador;
    }

    this->pelicula = &pelicula;
    this->sala = &sala;
    this->horaInicio = horaInicio;
    calcularHoraFin();
}

int Sesion::getId() const {
    return id;
}

const Pelicula* Sesion::getPelicula() const {
    return pelicula;
}

const Sala* Sesion::getSala() const {
    return sala;
}

std::tm Sesion::getHoraInicio() const {
    return horaInicio;
}

std::tm Sesion::getHoraFin() const {
    return horaFin;
}

void Sesion::setPelicula(const Pelicula& pelicula) {
    this->pelicula = &pelicula;
}

void Sesion::setSala(Sala& sala) {
    this->sala = &sala;
}

void Sesion::setHoraInicio(std::tm horaInicio) {
    this->horaInicio = horaInicio;
    calcularHoraFin();
}