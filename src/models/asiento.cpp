#include "models/asiento.hpp"

Asiento::Asiento(int f, int c){
    fila = f;
    columna = c;
    estado = LIBRE;
}

int Asiento::getFila() const{
    return fila;
}

int Asiento::getColumna() const{
    return columna;
}

Estado Asiento::getEstado() const{
    return estado;
}

bool Asiento::reservar()
{
    if(estado == LIBRE){
        estado = OCUPADO;
        return true;
    }else{
        return false;
    }
}

bool Asiento::liberar()
{
    if(estado == OCUPADO){
        estado = LIBRE;
        return true;
    }else{
        return false;
    }
}
