#ifndef RESERVA_HPP
#define RESERVA_HPP

class Reserva {
 private:
  int id;
  int idSesion;
  int fila;
  int columna;

 public:
  Reserva(int id, int idSesion, int fila, int columna);

  int getId() const;
  int getIdSesion() const;
  int getFila() const;
  int getColumna() const;
};

#endif