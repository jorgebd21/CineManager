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

 public:
  Reserva(int id, int idSesion, int fila, int columna,
          std::string estado = "PENDIENTE", std::time_t timestampCreacion = 0);

  int getId() const;
  int getIdSesion() const;
  int getFila() const;
  int getColumna() const;
  std::string getEstado() const;
  std::time_t getTimestampCreacion() const;

  void setEstado(std::string estado);
};

#endif