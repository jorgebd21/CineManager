#ifndef SALA_H
#define SALA_H
class Sala {
 private:
  int id;
  int cineId;
  int numeroSala;
  int capacidad;
  int filas;
  int columnas;

 public:
  Sala(int id, int cineId, int numeroSala, int filas, int columnas);

  int getId() const;
  int getCineId() const;
  int getNumeroSala() const;
  int getCapacidad() const;
  int getFilas() const;
  int getColumnas() const;

  void setNumeroSala(int numeroNuevo);
  void setFilas(int filas);
  void setColumnas(int columnas);
};

#endif