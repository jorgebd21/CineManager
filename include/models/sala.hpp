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
    int getFilas();
    int getColumnas();

    int getTotalAsientos();
    int getAsientosOcupados();
    int getAsientosLibres();

    bool reservarAsiento(int fila, int columna);
    bool liberarAsiento(int fila, int columna);
};

#endif