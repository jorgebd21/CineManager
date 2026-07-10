#include "db/datamanager.hpp"

// --- Cine ---
bool DataManager::crearCine(const Cine& cine) { return cineRepo.crear(cine); }
Cine DataManager::obtenerCine(int id) { return cineRepo.obtenerPorId(id); }
std::vector<Cine> DataManager::obtenerCines() { return cineRepo.obtenerTodos(); }
bool DataManager::actualizarCine(int id, const Cine& cine) { return cineRepo.actualizar(id, cine); }
bool DataManager::eliminarCine(int id) { return cineRepo.eliminar(id); }

// --- Sala ---
bool DataManager::crearSala(const Sala& sala) { return salaRepo.crear(sala); }
Sala DataManager::obtenerSala(int id) { return salaRepo.obtenerPorId(id); }
std::vector<Sala> DataManager::obtenerSalas() { return salaRepo.obtenerTodos(); }
std::vector<Sala> DataManager::obtenerSalasDeCine(int idCine) { return salaRepo.obtenerSalasDeCine(idCine); }
bool DataManager::actualizarSala(int id, const Sala& sala) { return salaRepo.actualizar(id, sala); }
bool DataManager::eliminarSala(int id) { return salaRepo.eliminar(id); }

// --- Pelicula ---
bool DataManager::crearPelicula(const Pelicula& pelicula) { return peliculaRepo.crear(pelicula); }
Pelicula DataManager::obtenerPelicula(int id) { return peliculaRepo.obtenerPorId(id); }
std::vector<Pelicula> DataManager::obtenerPeliculas() { return peliculaRepo.obtenerTodos(); }
std::vector<Pelicula> DataManager::obtenerCartelera(int idCine) { return peliculaRepo.obtenerCartelera(idCine); }
bool DataManager::actualizarPelicula(int id, const Pelicula& pelicula) { return peliculaRepo.actualizar(id, pelicula); }
bool DataManager::eliminarPelicula(int id) { return peliculaRepo.eliminar(id); }

// --- Sesion ---
bool DataManager::crearSesion(const Sesion& sesion) { return sesionRepo.crear(sesion); }
Sesion DataManager::obtenerSesion(int id) { return sesionRepo.obtenerPorId(id); }
std::vector<Sesion> DataManager::obtenerSesiones() { return sesionRepo.obtenerTodos(); }
std::vector<Sesion> DataManager::obtenerSesionesDeCine(int idCine) { return sesionRepo.obtenerSesionesDeCine(idCine); }
std::vector<Sesion> DataManager::obtenerSesionesDePelicula(int idCine, int idPelicula) { return sesionRepo.obtenerSesionesDePelicula(idCine, idPelicula); }
bool DataManager::actualizarSesion(int id, const Sesion& sesion) { return sesionRepo.actualizar(id, sesion); }
bool DataManager::eliminarSesion(int id) { return sesionRepo.eliminar(id); }

// --- Reserva ---
bool DataManager::crearReserva(const Reserva& reserva) { return reservaRepo.crear(reserva); }
Reserva DataManager::obtenerReserva(int id) { return reservaRepo.obtenerPorId(id); }
std::vector<Reserva> DataManager::obtenerReservasDeSesion(int idSesion) { return reservaRepo.obtenerPorSesion(idSesion); }
bool DataManager::actualizarReserva(int id, const Reserva& reserva) { return reservaRepo.actualizar(id, reserva); }
bool DataManager::eliminarReserva(int id) { return reservaRepo.eliminar(id); }