#include "models/sesion.hpp"

Sesion::Sesion(int id, Pelicula pelicula, int idSala, std::time_t horaInicio)
    : id(id), pelicula(pelicula), idSala(idSala), horaInicio(horaInicio) {
  horaFin = horaInicio + (pelicula.getDuracion() * 60);
}

int Sesion::getId() const { return id; }

Pelicula Sesion::getPelicula() const { return pelicula; }

int Sesion::getIdSala() const { return idSala; }

std::time_t Sesion::getHoraInicio() const { return horaInicio; }

std::time_t Sesion::getHoraFin() const { return horaFin; }

void Sesion::setPelicula(Pelicula pelicula) {
  this->pelicula = pelicula;
  horaFin = horaInicio + (this->pelicula.getDuracion() * 60);
}

void Sesion::setIdSala(int idSala) { this->idSala = idSala; }

void Sesion::setHoraInicio(std::time_t horaInicio) {
  this->horaInicio = horaInicio;
  horaFin = horaInicio + (pelicula.getDuracion() * 60);
}
