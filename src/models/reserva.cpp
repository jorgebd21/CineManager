#include "models/reserva.hpp"

Reserva::Reserva(int id, int idSesion, int fila, int columna)
    : id(id), idSesion(idSesion), fila(fila), columna(columna) {}

int Reserva::getId() const { return id; }

int Reserva::getIdSesion() const { return idSesion; }

int Reserva::getFila() const { return fila; }

int Reserva::getColumna() const { return columna; }
