#ifndef USUARIO_HPP
#define USUARIO_HPP

#include <string>

class Usuario {
 private:
  std::string dni;
  std::string nombre;
  std::string apellidos;
  std::string email;
  std::string passwordHash;
  std::string rol;

 public:
  Usuario() = default;
  Usuario(std::string dni, std::string nombre, std::string apellidos,
          std::string email, std::string passwordHash, std::string rol);

  std::string getDni() const;
  std::string getNombre() const;
  std::string getApellidos() const;
  std::string getEmail() const;
  std::string getPasswordHash() const;
  std::string getRol() const;

  bool esValido() const { return !dni.empty() && dni != "-1"; }

  void setDni(std::string dni) { this->dni = dni; }
  void setNombre(std::string nombre) { this->nombre = nombre; }
  void setApellidos(std::string apellidos) { this->apellidos = apellidos; }
  void setEmail(std::string email) { this->email = email; }
  void setPasswordHash(std::string passwordHash) {
    this->passwordHash = passwordHash;
  }
  void setRol(std::string rol) { this->rol = rol; }
};

#endif