#ifndef SESION_H
#define SESION_H

#include <ctime>
#include "pelicula.hpp"

class Sesion{
private:
    int id;
    const Pelicula* pelicula;
    int idSala;
    std::time_t horaInicio;
    std::time_t horaFin;
public:
    Sesion(int id, const Pelicula* pelicula, int idSala, std::time_t horaInicio);

    int getId() const;
    const Pelicula* getPelicula() const;
    int getIdSala() const;
    std::time_t getHoraInicio() const;
    std::time_t getHoraFin() const;

    void setPelicula(const Pelicula* pelicula);
    void setIdSala(int idSala);
    void setHoraInicio(std::time_t horaInicio);
};
#endif // SESION_H