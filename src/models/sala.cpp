#include "models/sala.hpp"

Sala::Sala(int f, int c) {
    filas = f;
    columnas = c;

    asientos = new Asiento*[filas];
    for (int i = 0; i < filas; i++) {
        asientos[i] = new Asiento[columnas]();
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

Asiento Sala::getAsiento(int f, int c) {
    if (!dentroRango(f, c, filas, columnas)) {
        return Asiento(-1, -1);
    }
    return asientos[f][c];
}

int Sala::getFilas() {
    return filas;
}

int Sala::getColumnas() {
    return columnas;
}

int Sala::getTotalAsientos() {
    return filas * columnas;
}

int Sala::getAsientosOcupados() {
    int count = 0;
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            if (asientos[i][j].getEstado() == Asiento::OCUPADO) {
                count++;
            }
        }
    }
    return count;
}

int Sala::getAsientosLibres() {
    return getTotalAsientos() - getAsientosOcupados();
}