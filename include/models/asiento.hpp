#ifndef ASIENTO_H
#define ASIENTO_H

class Asiento{
public:
    enum Estado {OCUPADO, LIBRE};
    Asiento(int f = 0, int c = 0);

    int getFila();
    int getColumna();
    Estado getEstado();

    bool reservar();
    bool liberar();
    
private:
    int fila;
    int columna;
    Estado estado;
};

#endif