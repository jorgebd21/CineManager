#include "models/cine.hpp"

Cine::Cine(int id, std::string nombre, std::string direccion)
    : id(id),
      nombre(std::move(nombre)),
      direccion(std::move(direccion)) {}

int Cine::getId() const noexcept { return id; }

const std::string& Cine::getNombre() const noexcept { return nombre; }

const std::string& Cine::getDireccion() const noexcept { return direccion; }

void Cine::setNombre(std::string nuevoNombre) noexcept {
  nombre = std::move(nuevoNombre);
}

void Cine::setDireccion(std::string nuevaDireccion) noexcept {
  direccion = std::move(nuevaDireccion);
}