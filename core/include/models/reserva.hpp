#ifndef RESERVA_HPP
#define RESERVA_HPP

#include <ctime>
#include <string>

class Reserva {
 private:
  int id;
  int idSesion;
  int fila;
  int columna;
  std::string estado;
  std::time_t timestampCreacion;
  std::string tipo;
  float precio;

 public:
  Reserva(int id, int idSesion, int fila, int columna,
          std::string estado = "PENDIENTE", std::time_t timestampCreacion = 0,
          std::string tipo = "Adulto", float precio = 7.5);

  int getId() const;
  int getIdSesion() const;
  int getFila() const;
  int getColumna() const;
  std::string getTipo() const;
  float getPrecio() const;
  std::string getEstado() const;
  std::time_t getTimestampCreacion() const;

  void setEstado(std::string estado);
  void setTipo(std::string tipo);
  void setPrecio(float precio);
};

#endif