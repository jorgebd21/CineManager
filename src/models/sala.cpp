#include "models/sala.hpp"

int Sala::contador = 0;

Sala::Sala(int f, int c) {
    id = ++contador;
    filas = f;
    columnas = c;
    estado = LIBRE;

    asientos.resize(filas);
    for (int i = 0; i < filas; i++) {
        asientos[i].resize(columnas);
    }

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            asientos[i][j] = Asiento(i, j);
        }
    }
}

bool dentroRango(int fila, int columna, int filas, int columnas) {
    return fila >= 0 && fila < filas && columna >= 0 && columna < columnas;
}

int Sala::getId() const {
    return id;
}

Asiento& Sala::getAsiento(int f, int c) {
    if (!dentroRango(f, c, filas, columnas)) {
        static Asiento dummy(-1, -1);
        return dummy;
    }
    return asientos[f][c];
}

const Asiento& Sala::getAsiento(int f, int c) const {
    if (!dentroRango(f, c, filas, columnas)) {
        static Asiento dummy(-1, -1);
        return dummy;
    }
    return asientos[f][c];
}

int Sala::getFilas() const{
    return filas;
}

int Sala::getColumnas() const{
    return columnas;
}

Estado Sala::getEstado() const {
    return estado;
}

Pelicula& Sala::getPelicula(){
    return pelicula;
}

const Pelicula& Sala::getPelicula() const {
    return pelicula;
}

int Sala::getTotalAsientos() const{
    return filas * columnas;
}

int Sala::getAsientosOcupados() const{
    int count = 0;
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            if (asientos[i][j].estaOcupado()) {
                count++;
            }
        }
    }
    return count;
}

int Sala::getAsientosLibres() const{
    return getTotalAsientos() - getAsientosOcupados();
}

bool Sala::reservarAsiento(int fila, int columna) {
    if (!dentroRango(fila, columna, filas, columnas) || estado == LIBRE) {
        return false;
    }
    return asientos[fila][columna].reservar();
}

bool Sala::liberarAsiento(int fila, int columna) {
    if (!dentroRango(fila, columna, filas, columnas)) {
        return false;
    }
    return asientos[fila][columna].liberar();
}

bool Sala::reservarSala(const Pelicula& pelicula) {
    if (estaOcupada()) {
        return false;
    }
    this->estado = OCUPADO;
    this->pelicula = pelicula;
    return true;
}

bool Sala::liberarSala() {
    if (!estaOcupada()) {
        return false;
    }
    this->estado = LIBRE;
    this->pelicula = Pelicula();
    return true;
}

bool Sala::estaOcupada() const {
    return estado == OCUPADO;
}