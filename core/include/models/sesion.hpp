#ifndef SESION_H
#define SESION_H

#include <ctime>
#include <utility>

#include "pelicula.hpp"

class Sesion {
 private:
  int id{-1};
  Pelicula pelicula;
  int idSala{-1};
  std::time_t horaInicio{0};
  std::time_t horaFin{0};

 public:
  Sesion() = default;
  Sesion(int id, Pelicula pelicula, int idSala, std::time_t horaInicio);

  [[nodiscard]] int getId() const noexcept;
  [[nodiscard]] const Pelicula& getPelicula() const noexcept;
  [[nodiscard]] int getIdSala() const noexcept;
  [[nodiscard]] std::time_t getHoraInicio() const noexcept;
  [[nodiscard]] std::time_t getHoraFin() const noexcept;
  [[nodiscard]] bool esValido() const noexcept { return id != -1; }

  void setPelicula(Pelicula nuevaPelicula) noexcept;
  void setIdSala(int nuevoIdSala) noexcept;
  void setHoraInicio(std::time_t nuevaHoraInicio) noexcept;
};

#endif  // SESION_H