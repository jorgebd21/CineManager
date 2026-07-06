#ifndef ASIENTO_H
#define ASIENTO_H
class Asiento{
private:
    int fila;
    int columna;

public:
    Asiento(int f = -1, int c = -1){
        fila = f;
        columna = c;
    };

    int getFila() const {return fila;};
    int getColumna() const {return columna;};
};

#endif