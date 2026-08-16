#include "models/sesion.hpp"

Sesion::Sesion(int id, Pelicula pelicula, int idSala, std::time_t horaInicio)
    : id(id),
      pelicula(std::move(pelicula)),
      idSala(idSala),
      horaInicio(horaInicio),
      horaFin(horaInicio + (this->pelicula.getDuracion() * 60)) {}

int Sesion::getId() const noexcept { return id; }

const Pelicula& Sesion::getPelicula() const noexcept { return pelicula; }

int Sesion::getIdSala() const noexcept { return idSala; }

std::time_t Sesion::getHoraInicio() const noexcept { return horaInicio; }

std::time_t Sesion::getHoraFin() const noexcept { return horaFin; }

void Sesion::setPelicula(Pelicula nuevaPelicula) noexcept {
  pelicula = std::move(nuevaPelicula);
  horaFin = horaInicio + (pelicula.getDuracion() * 60);
}

void Sesion::setIdSala(int nuevoIdSala) noexcept { idSala = nuevoIdSala; }

void Sesion::setHoraInicio(std::time_t nuevaHoraInicio) noexcept {
  horaInicio = nuevaHoraInicio;
  horaFin = horaInicio + (pelicula.getDuracion() * 60);
}
