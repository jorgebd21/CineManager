#include "models/usuario.hpp"

Usuario::Usuario(std::string dni, std::string nombre, std::string apellidos,
                 std::string email, std::string passwordHash, std::string rol)
    : dni(dni),
      nombre(nombre),
      apellidos(apellidos),
      email(email),
      passwordHash(passwordHash),
      rol(rol) {}

std::string Usuario::getDni() const { return dni; }
std::string Usuario::getNombre() const { return nombre; }
std::string Usuario::getApellidos() const { return apellidos; }
std::string Usuario::getEmail() const { return email; }
std::string Usuario::getPasswordHash() const { return passwordHash; }
std::string Usuario::getRol() const { return rol; }
