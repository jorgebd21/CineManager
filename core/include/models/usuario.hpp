#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>
#include <string_view>
#include <utility>

enum class Rol { CLIENTE, ADMIN };

[[nodiscard]] inline std::string rolToString(Rol rol) {
  switch (rol) {
    case Rol::ADMIN:
      return "ADMIN";
    case Rol::CLIENTE:
    default:
      return "CLIENTE";
  }
}

[[nodiscard]] inline Rol stringToRol(std::string_view rolStr) {
  if (rolStr == "ADMIN" || rolStr == "admin") {
    return Rol::ADMIN;
  }
  return Rol::CLIENTE;
}

class Usuario {
 private:
  std::string dni;
  std::string nombre;
  std::string apellidos;
  std::string email;
  std::string passwordHash;
  std::string rol{"CLIENTE"};

 public:
  Usuario() = default;
  Usuario(std::string dni, std::string nombre, std::string apellidos,
          std::string email, std::string passwordHash, std::string rol = "CLIENTE");

  [[nodiscard]] const std::string& getDni() const noexcept;
  [[nodiscard]] const std::string& getNombre() const noexcept;
  [[nodiscard]] const std::string& getApellidos() const noexcept;
  [[nodiscard]] const std::string& getEmail() const noexcept;
  [[nodiscard]] const std::string& getPasswordHash() const noexcept;
  [[nodiscard]] const std::string& getRol() const noexcept;
  [[nodiscard]] Rol getRolEnum() const noexcept;

  [[nodiscard]] bool esValido() const noexcept {
    return !dni.empty() && dni != "-1";
  }

  void setDni(std::string nuevoDni) noexcept { dni = std::move(nuevoDni); }
  void setNombre(std::string nuevoNombre) noexcept {
    nombre = std::move(nuevoNombre);
  }
  void setApellidos(std::string nuevosApellidos) noexcept {
    apellidos = std::move(nuevosApellidos);
  }
  void setEmail(std::string nuevoEmail) noexcept {
    email = std::move(nuevoEmail);
  }
  void setPasswordHash(std::string nuevoHash) noexcept {
    passwordHash = std::move(nuevoHash);
  }
  void setRol(std::string nuevoRol) noexcept { rol = std::move(nuevoRol); }
  void setRolEnum(Rol nuevoRol) noexcept { rol = rolToString(nuevoRol); }
};

#endif  // USUARIO_HPP