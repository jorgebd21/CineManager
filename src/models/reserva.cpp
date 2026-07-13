#include "models/reserva.hpp"

Reserva::Reserva(int id, int idSesion, int fila, int columna,
                 std::string estado, std::time_t timestampCreacion)
    : id(id),
      idSesion(idSesion),
      fila(fila),
      columna(columna),
      estado(estado),
      timestampCreacion(timestampCreacion) {}

int Reserva::getId() const { return id; }

int Reserva::getIdSesion() const { return idSesion; }

int Reserva::getFila() const { return fila; }

int Reserva::getColumna() const { return columna; }

std::string Reserva::getEstado() const { return estado; }

std::time_t Reserva::getTimestampCreacion() const { return timestampCreacion; }

void Reserva::setEstado(std::string estado) { this->estado = estado; }
