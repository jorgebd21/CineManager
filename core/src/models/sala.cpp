#include "models/sala.hpp"

Sala::Sala(int id, int cineId, int numeroSala, int filas, int columnas)
    : id(id),
      cineId(cineId),
      numeroSala(numeroSala),
      filas(filas),
      columnas(columnas) {
  capacidad = filas * columnas;
}

int Sala::getId() const { return id; }

int Sala::getCineId() const { return cineId; }

int Sala::getNumeroSala() const { return numeroSala; }

int Sala::getCapacidad() const { return capacidad; }

int Sala::getFilas() const { return filas; }

int Sala::getColumnas() const { return columnas; }

void Sala::setNumeroSala(int numeroNuevo) { numeroSala = numeroNuevo; }

void Sala::setFilas(int filas) {
  this->filas = filas;
  capacidad = filas * columnas;
}

void Sala::setColumnas(int columnas) {
  this->columnas = columnas;
  capacidad = filas * columnas;
}
