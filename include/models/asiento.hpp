#ifndef ASIENTO_H
#define ASIENTO_H

enum Estado {OCUPADO, LIBRE};
class Asiento{
private:
    int fila;
    int columna;
    Estado estado;

public:
    Asiento(int f = 0, int c = 0);

    int getFila();
    int getColumna();
    Estado getEstado();

    bool reservar();
    bool liberar();
};

#endif