#ifndef CINE_H
#define CINE_H

#include <string>
#include <utility>

class Cine {
 private:
  int id{-1};
  std::string nombre;
  std::string direccion;

 public:
  Cine() = default;
  Cine(int id, std::string nombre, std::string direccion);

  [[nodiscard]] int getId() const noexcept;
  [[nodiscard]] const std::string& getNombre() const noexcept;
  [[nodiscard]] const std::string& getDireccion() const noexcept;
  [[nodiscard]] bool esValido() const noexcept { return id != -1; }

  void setNombre(std::string nuevoNombre) noexcept;
  void setDireccion(std::string nuevaDireccion) noexcept;
};

#endif  // CINE_H