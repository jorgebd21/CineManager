#ifndef RESERVA_HPP
#define RESERVA_HPP

#include <ctime>
#include <string>
#include <string_view>
#include <utility>

enum class EstadoReserva { PENDIENTE, COMPRADO, CANCELADA, ANALIZANDO };

[[nodiscard]] inline std::string estadoReservaToString(EstadoReserva estado) {
  switch (estado) {
    case EstadoReserva::COMPRADO:
      return "COMPRADO";
    case EstadoReserva::CANCELADA:
      return "CANCELADA";
    case EstadoReserva::ANALIZANDO:
      return "ANALIZANDO";
    case EstadoReserva::PENDIENTE:
    default:
      return "PENDIENTE";
  }
}

[[nodiscard]] inline EstadoReserva stringToEstadoReserva(std::string_view str) {
  if (str == "COMPRADO" || str == "CONFIRMADA" || str == "confirmada" || str == "comprado") {
    return EstadoReserva::COMPRADO;
  }
  if (str == "CANCELADA" || str == "cancelada") {
    return EstadoReserva::CANCELADA;
  }
  if (str == "ANALIZANDO") {
    return EstadoReserva::ANALIZANDO;
  }
  return EstadoReserva::PENDIENTE;
}

class Reserva {
 private:
  int id{-1};
  int idSesion{-1};
  int fila{-1};
  int columna{-1};
  std::string estado{"PENDIENTE"};
  std::time_t timestampCreacion{0};
  std::string tipo{"Adulto"};
  float precio{7.50f};

 public:
  Reserva() = default;
  Reserva(int id, int idSesion, int fila, int columna,
          std::string estado = "PENDIENTE", std::time_t timestampCreacion = 0,
          std::string tipo = "Adulto", float precio = 7.50f);

  [[nodiscard]] int getId() const noexcept;
  [[nodiscard]] int getIdSesion() const noexcept;
  [[nodiscard]] int getFila() const noexcept;
  [[nodiscard]] int getColumna() const noexcept;
  [[nodiscard]] const std::string& getTipo() const noexcept;
  [[nodiscard]] float getPrecio() const noexcept;
  [[nodiscard]] const std::string& getEstado() const noexcept;
  [[nodiscard]] EstadoReserva getEstadoEnum() const noexcept;
  [[nodiscard]] std::time_t getTimestampCreacion() const noexcept;
  [[nodiscard]] bool esValido() const noexcept { return id != -1; }

  void setEstado(std::string nuevoEstado) noexcept;
  void setEstadoEnum(EstadoReserva nuevoEstado) noexcept;
  void setTipo(std::string nuevoTipo) noexcept;
  void setPrecio(float nuevoPrecio) noexcept;
  void setId(int nuevoId) noexcept { id = nuevoId; }
};

#endif  // RESERVA_HPP