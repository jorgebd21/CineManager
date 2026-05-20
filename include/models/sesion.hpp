#ifndef SESION_H
#define SESION_H

#include <ctime>
#include "pelicula.hpp"
#include "sala.hpp"

class Sesion{
private:
    static int contador;

    void calcularHoraFin();

    int id;
    const class Pelicula *pelicula;
    class Sala *sala;
    std::tm horaInicio;
    std::tm horaFin;
public:
    Sesion(const Pelicula &pelicula, Sala &sala, std::tm horaInicio);

    int getId() const;
    const Pelicula* getPelicula() const;
    const Sala* getSala() const;
    std::tm getHoraInicio() const;
    std::tm getHoraFin() const;

    void setPelicula(const Pelicula& pelicula);
    void setSala(Sala& sala);
    void setHoraInicio(std::tm horaInicio);
};
#endif // SESION_H