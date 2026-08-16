#ifndef PELICULA_H
#define PELICULA_H

#include <string>
#include <string_view>
#include <utility>

enum class Genero {
  NONE,
  ACCION,
  COMEDIA,
  DRAMA,
  TERROR,
  CIENCIA_FICCION,
  ROMANCE,
  DOCUMENTAL,
  OTHER
};

[[nodiscard]] Genero intToGenero(int generoN) noexcept;
[[nodiscard]] Genero stringToGenero(std::string_view generoStr) noexcept;
[[nodiscard]] std::string generoToString(Genero genero);

class Pelicula {
 private:
  int id{-1};
  std::string titulo;
  Genero genero{Genero::NONE};
  int duracion{0};

 public:
  Pelicula() = default;
  Pelicula(int id, std::string titulo, Genero genero, int duracion);

  [[nodiscard]] int getId() const noexcept;
  [[nodiscard]] const std::string& getTitulo() const noexcept;
  [[nodiscard]] Genero getGenero() const noexcept;
  [[nodiscard]] int getDuracion() const noexcept;
  [[nodiscard]] bool esValido() const noexcept { return id != -1; }

  void setTitulo(std::string nuevoTitulo) noexcept;
  void setGenero(Genero nuevoGenero) noexcept;
  void setDuracion(int nuevaDuracion) noexcept;
};

#endif  // PELICULA_H