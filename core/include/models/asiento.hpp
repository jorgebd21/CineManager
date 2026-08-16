#ifndef ASIENTO_H
#define ASIENTO_H

#include <compare>

class Asiento {
 private:
  int fila{-1};
  int columna{-1};

 public:
  constexpr Asiento() noexcept = default;
  constexpr explicit Asiento(int f, int c) noexcept : fila(f), columna(c) {}

  [[nodiscard]] constexpr int getFila() const noexcept { return fila; }
  [[nodiscard]] constexpr int getColumna() const noexcept { return columna; }

  [[nodiscard]] constexpr bool esValido() const noexcept {
    return fila >= 0 && columna >= 0;
  }

  auto operator<=>(const Asiento&) const noexcept = default;
};

#endif  // ASIENTO_H