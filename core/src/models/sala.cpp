#include "models/sala.hpp"

Sala::Sala(int id, int cineId, int numeroSala, int filas, int columnas)
    : id(id),
      cineId(cineId),
      numeroSala(numeroSala),
      filas(filas),
      columnas(columnas) {
  capacidad = filas * columnas;
}

int Sala::getId() const noexcept { return id; }

int Sala::getCineId() const noexcept { return cineId; }

int Sala::getNumeroSala() const noexcept { return numeroSala; }

int Sala::getCapacidad() const noexcept { return capacidad; }

int Sala::getFilas() const noexcept { return filas; }

int Sala::getColumnas() const noexcept { return columnas; }

void Sala::setNumeroSala(int numeroNuevo) noexcept { numeroSala = numeroNuevo; }

void Sala::setFilas(int nuevasFilas) noexcept {
  filas = nuevasFilas;
  capacidad = filas * columnas;
}

void Sala::setColumnas(int nuevasColumnas) noexcept {
  columnas = nuevasColumnas;
  capacidad = filas * columnas;
}
