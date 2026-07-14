#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include <sqlite3.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "db/database.hpp"
#include "db/repositories/cinerepository.hpp"
#include "db/repositories/pelicularepository.hpp"
#include "db/repositories/reservarepository.hpp"
#include "db/repositories/salarepository.hpp"
#include "db/repositories/sesionrepository.hpp"
#include "models/cine.hpp"
#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include "models/sala.hpp"
#include "models/sesion.hpp"
class DataManager {
 private:
  static constexpr int TIEMPO_EXPIRACION_SEGUNDOS = 300;

  SqliteDatabase db;

  std::unordered_map<int, std::unique_ptr<std::mutex>> sessionMutexes;
  std::mutex mapMutex;

  std::thread cleanerThread;
  std::atomic<bool> stopCleaner{false};
  std::condition_variable cvCleaner;
  std::mutex cvMutex;

  CineRepository cineRepo;
  PeliculaRepository peliculaRepo;
  SalaRepository salaRepo;
  SesionRepository sesionRepo;
  ReservaRepository reservaRepo;

 public:
  DataManager()
      : db(),
        cineRepo(db),
        peliculaRepo(db),
        salaRepo(db),
        sesionRepo(db),
        reservaRepo(db) {
    cleanerThread = std::thread(&DataManager::iniciarLimpiezaLoop, this);
  };
  ~DataManager();

  void iniciarLimpiezaLoop();
  std::vector<Reserva> obtenerPendientes();

  int crearCine(const Cine& cine);
  int crearSala(const Sala& sala);
  int crearPelicula(const Pelicula& pelicula);
  int crearSesion(const Sesion& sesion);
  int crearReserva(const Reserva& reserva);

  Cine obtenerCine(int id);
  Sala obtenerSala(int id);
  Pelicula obtenerPelicula(int id);
  Sesion obtenerSesion(int id);
  Reserva obtenerReserva(int id);

  std::mutex& obtenerMutexSesion(int idSesion);

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

#endif  // DATAMANAGER_HPP