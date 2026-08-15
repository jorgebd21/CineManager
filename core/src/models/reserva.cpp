#include "models/reserva.hpp"

Reserva::Reserva(int id, int idSesion, int fila, int columna,
                 std::string estado, std::time_t timestampCreacion,
                 std::string tipo, float precio)
    : id(id),
      idSesion(idSesion),
      fila(fila),
      columna(columna),
      estado(estado),
      timestampCreacion(timestampCreacion),
      tipo(tipo),
      precio(precio) {}

int Reserva::getId() const { return id; }

int Reserva::getIdSesion() const { return idSesion; }

int Reserva::getFila() const { return fila; }

int Reserva::getColumna() const { return columna; }

std::string Reserva::getTipo() const { return tipo; }

float Reserva::getPrecio() const { return precio; }

std::string Reserva::getEstado() const { return estado; }

std::time_t Reserva::getTimestampCreacion() const { return timestampCreacion; }

void Reserva::setEstado(std::string estado) { this->estado = estado; }

void Reserva::setTipo(std::string tipo) { this->tipo = tipo; }

void Reserva::setPrecio(float precio) { this->precio = precio; }