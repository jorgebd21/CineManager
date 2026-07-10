#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include <sqlite3.h>
#include <vector>

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

    bool crearCine(const Cine& cine);
    bool crearSala(const Sala& sala);
    bool crearPelicula(const Pelicula& pelicula);
    bool crearSesion(const Sesion& sesion);
    bool crearReserva(const Reserva& reserva);

    Cine obtenerCine(int id);
    Sala obtenerSala(int id);
    Pelicula obtenerPelicula(int id);
    Sesion obtenerSesion(int id);
    Reserva obtenerReserva(int id);

    std::vector<Pelicula> obtenerCartelera(int idCine);
    std::vector<Sesion> obtenerSesionesDePelicula(int idCine, int idPelicula);
    std::vector<Reserva> obtenerReservasDeSesion(int idSesion);
    std::vector<Sala> obtenerSalasDeCine(int idCine);
    std::vector<Pelicula> obtenerPeliculas();
    std::vector<Sesion> obtenerSesionesDeCine(int idCine);
    std::vector<Cine> obtenerCines();
    std::vector<Sala> obtenerSalas();
    std::vector<Sesion> obtenerSesiones();

    bool actualizarCine(int id, const Cine& cine);
    bool actualizarSala(int id, const Sala& sala);
    bool actualizarPelicula(int id, const Pelicula& pelicula);
    bool actualizarSesion(int id, const Sesion& sesion);
    bool actualizarReserva(int id, const Reserva& reserva);

    bool eliminarCine(int id);
    bool eliminarSala(int id);
    bool eliminarPelicula(int id);
    bool eliminarSesion(int id);
    bool eliminarReserva(int id);
};

#endif // DATAMANAGER_HPP