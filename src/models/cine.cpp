#include "models/cine.hpp"

Cine::Cine(int id, const std::string& nombre, const std::string& direccion) 
    : id(id), nombre(nombre), direccion(direccion) {}

int Cine::getId() const {
    return id;
}

std::string Cine::getNombre() const {
    return nombre;
}

std::string Cine::getDireccion() const {
    return direccion;
}

void Cine::setNombre(const std::string& nombre) {
    this->nombre = nombre;
}

void Cine::setDireccion(const std::string& direccion) {
    this->direccion = direccion;
}