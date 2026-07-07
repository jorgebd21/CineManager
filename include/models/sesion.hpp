#ifndef SESION_H
#define SESION_H

#include <ctime>
#include "pelicula.hpp"

class Sesion{
private:
    int id;
    Pelicula pelicula;
    int idSala;
    std::time_t horaInicio;
    std::time_t horaFin;
public:
    Sesion(int id, Pelicula pelicula, int idSala, std::time_t horaInicio);

    int getId() const;
    Pelicula getPelicula() const;
    int getIdSala() const;
    std::time_t getHoraInicio() const;
    std::time_t getHoraFin() const;

    void setPelicula(Pelicula pelicula);
    void setIdSala(int idSala);
    void setHoraInicio(std::time_t horaInicio);
};
#endif // SESION_H