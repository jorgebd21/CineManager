#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include <sqlite3.h>

#include "models/cine.hpp"
#include "models/sala.hpp"
#include "models/pelicula.hpp"
#include "models/sesion.hpp"
#include "models/reserva.hpp"

class DataManager {
private:
    sqlite3* db;

public:
    DataManager(){};
    bool abrirSQL();
    void cerrarSQL();

    bool crearCine(Cine cine);
    bool crearSala(Sala sala);
    bool crearPelicula(Pelicula pelicula);
    bool crearSesion(Sesion sesion);
    bool crearReserva(Reserva reserva);

    Cine obtenerCine(int id);
    Sala obtenerSala(int id);
    Pelicula obtenerPelicula(int id);
    Sesion obtenerSesion(int id);
    Reserva obtenerReserva(int id);

    bool actualizarCine(int id, Cine cine);
    bool actualizarSala(int id, Sala sala);
    bool actualizarPelicula(int id, Pelicula pelicula);
    bool actualizarSesion(int id, Sesion sesion);
    bool actualizarReserva(int id, Reserva reserva);

    bool eliminarCine(int id);
    bool eliminarSala(int id);
    bool eliminarPelicula(int id);
    bool eliminarSesion(int id);
    bool eliminarReserva(int id);
};

#endif // DATAMANAGER_HPP