#ifndef SALA_H
#define SALA_H

class Sala {
 private:
  int id{-1};
  int cineId{-1};
  int numeroSala{0};
  int capacidad{0};
  int filas{0};
  int columnas{0};

 public:
  Sala() = default;
  Sala(int id, int cineId, int numeroSala, int filas, int columnas);

  [[nodiscard]] int getId() const noexcept;
  [[nodiscard]] int getCineId() const noexcept;
  [[nodiscard]] int getNumeroSala() const noexcept;
  [[nodiscard]] int getCapacidad() const noexcept;
  [[nodiscard]] int getFilas() const noexcept;
  [[nodiscard]] int getColumnas() const noexcept;
  [[nodiscard]] bool esValido() const noexcept { return id != -1; }

  void setNumeroSala(int numeroNuevo) noexcept;
  void setFilas(int filas) noexcept;
  void setColumnas(int columnas) noexcept;
};

#endif  // SALA_H