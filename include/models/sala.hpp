#ifndef SALA_H
#define SALA_H

#include <vector>
#include "models/asiento.hpp"
#include "models/pelicula.hpp"
class Sala {
private:
    static int contador;

    int id;
    int filas;
    int columnas;
    std::vector<std::vector<Asiento>> asientos;
    Estado estado;
    Pelicula pelicula;

public:
    Sala(int f=0, int c=0);

    int getId() const;
    Asiento getAsiento(int f, int j);
    int getFilas() const;
    int getColumnas() const;
    Estado getEstado() const;
    Pelicula getPelicula() const;

    int getTotalAsientos() const;
    int getAsientosOcupados() const;
    int getAsientosLibres() const;

    bool reservarAsiento(int fila, int columna);
    bool liberarAsiento(int fila, int columna);
    
    bool reservarSala(const Pelicula& pelicula);
    bool liberarSala();
};

#endif