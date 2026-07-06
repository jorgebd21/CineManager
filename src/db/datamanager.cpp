#include "db/datamanager.hpp"
#include <iostream>

using namespace std;

bool DataManager::abrirSQL() {
    int rc = sqlite3_open("../data/cine.db", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "No se pudo abrir: " << sqlite3_errmsg(db) << std::endl;
    }
    return rc == SQLITE_OK;
}

void DataManager::cerrarSQL() {
    if(db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool DataManager::crearCine(const Cine& cine) { return true; }
bool DataManager::crearSala(const Sala& sala) { return true; }
bool DataManager::crearPelicula(const Pelicula& pelicula) { return true; }
bool DataManager::crearSesion(const Sesion& sesion) { return true; }
bool DataManager::crearReserva(const Reserva& reserva) { return true; }

static Pelicula mockPelicula(1, "Pelicula Mock", Genero::NONE, 120);

Cine DataManager::obtenerCine(int id) { return Cine(id, "Cine Mock", "Direccion Mock"); }
Sala DataManager::obtenerSala(int id) { return Sala(id, 1, 1, 5, 7); }
Pelicula DataManager::obtenerPelicula(int id) { return mockPelicula; }
Sesion DataManager::obtenerSesion(int id) { return Sesion(id, &mockPelicula, 1, 0); }
Reserva DataManager::obtenerReserva(int id) { return Reserva(id, 1, 1, 1); }

std::vector<Pelicula> DataManager::obtenerCartelera(int idCine) { return {}; }
std::vector<Sesion> DataManager::obtenerSesionesDePelicula(int idPelicula) { return {}; }
std::vector<Reserva> DataManager::obtenerReservasDeSesion(int idSesion) { return {}; }
std::vector<Sala> DataManager::obtenerSalasDeCine(int idCine) { return {}; }

bool DataManager::actualizarCine(int id, const Cine& cine) { return true; }
bool DataManager::actualizarSala(int id, const Sala& sala) { return true; }
bool DataManager::actualizarPelicula(int id, const Pelicula& pelicula) { return true; }
bool DataManager::actualizarSesion(int id, const Sesion& sesion) { return true; }
bool DataManager::actualizarReserva(int id, const Reserva& reserva) { return true; }

bool DataManager::eliminarCine(int id) { return true; }
bool DataManager::eliminarSala(int id) { return true; }
bool DataManager::eliminarPelicula(int id) { return true; }
bool DataManager::eliminarSesion(int id) { return true; }
bool DataManager::eliminarReserva(int id) { return true; }