#include "models/reserva.hpp"

Reserva::Reserva(int id, int idSesion, int fila, int columna,
                 std::string estado, std::time_t timestampCreacion,
                 std::string tipo, float precio)
    : id(id),
      idSesion(idSesion),
      fila(fila),
      columna(columna),
      estado(std::move(estado)),
      timestampCreacion(timestampCreacion),
      tipo(std::move(tipo)),
      precio(precio) {}

int Reserva::getId() const noexcept { return id; }

int Reserva::getIdSesion() const noexcept { return idSesion; }

int Reserva::getFila() const noexcept { return fila; }

int Reserva::getColumna() const noexcept { return columna; }

const std::string& Reserva::getTipo() const noexcept { return tipo; }

float Reserva::getPrecio() const noexcept { return precio; }

const std::string& Reserva::getEstado() const noexcept { return estado; }

EstadoReserva Reserva::getEstadoEnum() const noexcept {
  return stringToEstadoReserva(estado);
}

std::time_t Reserva::getTimestampCreacion() const noexcept {
  return timestampCreacion;
}

void Reserva::setEstado(std::string nuevoEstado) noexcept {
  estado = std::move(nuevoEstado);
}

void Reserva::setEstadoEnum(EstadoReserva nuevoEstado) noexcept {
  estado = estadoReservaToString(nuevoEstado);
}

void Reserva::setTipo(std::string nuevoTipo) noexcept {
  tipo = std::move(nuevoTipo);
}

void Reserva::setPrecio(float nuevoPrecio) noexcept {
  precio = nuevoPrecio;
}