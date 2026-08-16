#include "models/usuario.hpp"

Usuario::Usuario(std::string dni, std::string nombre, std::string apellidos,
                 std::string email, std::string passwordHash, std::string rol)
    : dni(std::move(dni)),
      nombre(std::move(nombre)),
      apellidos(std::move(apellidos)),
      email(std::move(email)),
      passwordHash(std::move(passwordHash)),
      rol(std::move(rol)) {}

const std::string& Usuario::getDni() const noexcept { return dni; }
const std::string& Usuario::getNombre() const noexcept { return nombre; }
const std::string& Usuario::getApellidos() const noexcept { return apellidos; }
const std::string& Usuario::getEmail() const noexcept { return email; }
const std::string& Usuario::getPasswordHash() const noexcept {
  return passwordHash;
}
const std::string& Usuario::getRol() const noexcept { return rol; }

Rol Usuario::getRolEnum() const noexcept { return stringToRol(rol); }
