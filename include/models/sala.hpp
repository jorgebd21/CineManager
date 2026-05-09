#ifndef SALA_H
#define SALA_H

#include "models/asiento.hpp"
class Sala {
private:
    int filas;
    int columnas;
    Asiento** asientos;

public:
    Sala(int f, int c);

    Asiento getAsiento(int f, int j);
    int getFilas() const;
    int getColumnas() const;

    int getTotalAsientos() const;
    int getAsientosOcupados() const;
    int getAsientosLibres() const;

    bool reservarAsiento(int fila, int columna);
    bool liberarAsiento(int fila, int columna);
};

#endif