#include "models/sala.hpp"

Sala::Sala(int id, int cineId, int numeroSala, int filas, int columnas) 
    : id(id), cineId(cineId), numeroSala(numeroSala), filas(filas), columnas(columnas) {}

int Sala::getId() const {
    return id;
}

int Sala::getCineId() const {
    return cineId;
}

int Sala::getNumeroSala() const {
    return numeroSala;
}

int Sala::getFilas() const {
    return filas;
}

int Sala::getColumnas() const {
    return columnas;
}

void Sala::setNumeroSala(int numeroSala) {
    this->numeroSala = numeroSala;
}

void Sala::setFilas(int filas) {
    this->filas = filas;
}

void Sala::setColumnas(int columnas) {
    this->columnas = columnas;
}
