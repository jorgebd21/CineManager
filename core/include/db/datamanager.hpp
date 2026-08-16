#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include <sqlite3.h>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "db/database.hpp"
#include "db/repositories/cinerepository.hpp"
#include "db/repositories/pelicularepository.hpp"
#include "db/repositories/reservarepository.hpp"
#include "db/repositories/salarepository.hpp"
#include "db/repositories/sesionrepository.hpp"
#include "db/repositories/usuariorepository.hpp"
#include "models/cine.hpp"
#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include "models/sala.hpp"
#include "models/sesion.hpp"
#include "models/usuario.hpp"

class DataManager {
 private:
  static constexpr int TIEMPO_EXPIRACION_SEGUNDOS = 300;

  SqliteDatabase db;

  std::unordered_map<int, std::shared_ptr<std::mutex>> sessionMutexes;
  std::mutex mapMutex;

  CineRepository cineRepo;
  PeliculaRepository peliculaRepo;
  SalaRepository salaRepo;
  SesionRepository sesionRepo;
  ReservaRepository reservaRepo;
  UsuarioRepository usuarioRepo;

  std::mutex cvMutex;
  std::condition_variable cvCleaner;
  std::jthread cleanerThread;

  void iniciarLimpiezaLoop(std::stop_token stoken);

 public:
  explicit DataManager(const std::string& customDbPath = "");
  ~DataManager();

  void detenerDemonioLimpieza();

  // Non-copyable, non-movable facade
  DataManager(const DataManager&) = delete;
  DataManager& operator=(const DataManager&) = delete;
  DataManager(DataManager&&) = delete;
  DataManager& operator=(DataManager&&) = delete;

  std::vector<Reserva> obtenerPendientes();

  int crearCine(const Cine& cine);
  int crearSala(const Sala& sala);
  int crearPelicula(const Pelicula& pelicula);
  int crearSesion(const Sesion& sesion);
  int crearReserva(const Reserva& reserva);
  bool crearReservasMultiples(int idSesion, const std::vector<Reserva>& reservas);

  Cine obtenerCine(int id);
  Sala obtenerSala(int id);
  Pelicula obtenerPelicula(int id);
  Sesion obtenerSesion(int id);
  Reserva obtenerReserva(int id);

  std::shared_ptr<std::mutex> obtenerMutexSesion(int idSesion);

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

  bool crearUsuario(const Usuario& usuario);
  Usuario obtenerUsuario(const std::string& dni);
  Usuario autenticarUsuario(const std::string& dni, const std::string& password);
  bool actualizarUsuario(const Usuario& usuario);
  bool eliminarUsuario(const std::string& dni);
};

#endif  // DATAMANAGER_HPP