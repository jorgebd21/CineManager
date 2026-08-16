#include "db/datamanager.hpp"

#include <ctime>
#include <iostream>

DataManager::DataManager(const std::string& customDbPath)
    : db(customDbPath),
      cineRepo(db),
      peliculaRepo(db),
      salaRepo(db),
      sesionRepo(db),
      reservaRepo(db),
      usuarioRepo(db) {
  cleanerThread = std::jthread([this](std::stop_token stoken) {
    iniciarLimpiezaLoop(stoken);
  });
}

DataManager::~DataManager() {
  if (cleanerThread.joinable()) {
    cleanerThread.request_stop();
    cvCleaner.notify_all();
    cleanerThread.join();
  }
}

std::shared_ptr<std::mutex> DataManager::obtenerMutexSesion(int idSesion) {
  std::lock_guard<std::mutex> lock(mapMutex);
  auto it = sessionMutexes.find(idSesion);
  if (it == sessionMutexes.end()) {
    auto mtx = std::make_shared<std::mutex>();
    sessionMutexes[idSesion] = mtx;
    return mtx;
  }
  return it->second;
}

std::vector<Reserva> DataManager::obtenerPendientes() {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return reservaRepo.obtenerPendientes();
}

void DataManager::iniciarLimpiezaLoop(std::stop_token stoken) {
  while (!stoken.stop_requested()) {
    {
      std::unique_lock<std::mutex> lock(cvMutex);
      cvCleaner.wait_for(lock, std::chrono::seconds(5),
                         [&stoken]() { return stoken.stop_requested(); });
    }

    if (stoken.stop_requested()) {
      break;
    }

    const std::time_t ahora = std::time(nullptr);
    std::vector<Reserva> reservas = obtenerPendientes();
    for (const auto& reserva : reservas) {
      if (TIEMPO_EXPIRACION_SEGUNDOS <= ahora - reserva.getTimestampCreacion()) {
        auto sesionMtx = obtenerMutexSesion(reserva.getIdSesion());
        std::lock_guard<std::mutex> lockSesion(*sesionMtx);
        std::lock_guard<std::mutex> lockDb(db.getMutex());
        reservaRepo.eliminar(reserva.getId());
      }
    }
  }
}

// --- Cine ---
int DataManager::crearCine(const Cine& cine) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return cineRepo.crear(cine);
}

Cine DataManager::obtenerCine(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return cineRepo.obtenerPorId(id);
}

std::vector<Cine> DataManager::obtenerCines() {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return cineRepo.obtenerTodos();
}

bool DataManager::actualizarCine(int id, const Cine& cine) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return cineRepo.actualizar(id, cine);
}

bool DataManager::eliminarCine(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return cineRepo.eliminar(id);
}

// --- Sala ---
int DataManager::crearSala(const Sala& sala) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return salaRepo.crear(sala);
}

Sala DataManager::obtenerSala(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return salaRepo.obtenerPorId(id);
}

std::vector<Sala> DataManager::obtenerSalas() {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return salaRepo.obtenerTodos();
}

std::vector<Sala> DataManager::obtenerSalasDeCine(int idCine) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return salaRepo.obtenerSalasDeCine(idCine);
}

bool DataManager::actualizarSala(int id, const Sala& sala) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return salaRepo.actualizar(id, sala);
}

bool DataManager::eliminarSala(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return salaRepo.eliminar(id);
}

// --- Pelicula ---
int DataManager::crearPelicula(const Pelicula& pelicula) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return peliculaRepo.crear(pelicula);
}

Pelicula DataManager::obtenerPelicula(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return peliculaRepo.obtenerPorId(id);
}

std::vector<Pelicula> DataManager::obtenerPeliculas() {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return peliculaRepo.obtenerTodos();
}

std::vector<Pelicula> DataManager::obtenerCartelera(int idCine) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return peliculaRepo.obtenerCartelera(idCine);
}

bool DataManager::actualizarPelicula(int id, const Pelicula& pelicula) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return peliculaRepo.actualizar(id, pelicula);
}

bool DataManager::eliminarPelicula(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return peliculaRepo.eliminar(id);
}

// --- Sesion ---
int DataManager::crearSesion(const Sesion& sesion) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return sesionRepo.crear(sesion);
}

Sesion DataManager::obtenerSesion(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return sesionRepo.obtenerPorId(id);
}

std::vector<Sesion> DataManager::obtenerSesiones() {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return sesionRepo.obtenerTodos();
}

std::vector<Sesion> DataManager::obtenerSesionesDeCine(int idCine) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return sesionRepo.obtenerSesionesDeCine(idCine);
}

std::vector<Sesion> DataManager::obtenerSesionesDePelicula(int idCine,
                                                           int idPelicula) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return sesionRepo.obtenerSesionesDePelicula(idCine, idPelicula);
}

bool DataManager::actualizarSesion(int id, const Sesion& sesion) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return sesionRepo.actualizar(id, sesion);
}

bool DataManager::eliminarSesion(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return sesionRepo.eliminar(id);
}

// --- Reserva ---
int DataManager::crearReserva(const Reserva& reserva) {
  auto sesionMtx = obtenerMutexSesion(reserva.getIdSesion());
  std::lock_guard<std::mutex> lockSesion(*sesionMtx);
  std::lock_guard<std::mutex> lockDb(db.getMutex());

  // Inserción directa con control atómico de clave única
  return reservaRepo.crear(reserva);
}

bool DataManager::crearReservasMultiples(int idSesion, const std::vector<Reserva>& reservas) {
  auto sesionMtx = obtenerMutexSesion(idSesion);
  std::lock_guard<std::mutex> lockSesion(*sesionMtx);
  std::lock_guard<std::mutex> lockDb(db.getMutex());

  return reservaRepo.crearMultiples(reservas);
}

Reserva DataManager::obtenerReserva(int id) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return reservaRepo.obtenerPorId(id);
}

std::vector<Reserva> DataManager::obtenerReservasDeSesion(int idSesion) {
  auto sesionMtx = obtenerMutexSesion(idSesion);
  std::lock_guard<std::mutex> lockSesion(*sesionMtx);
  std::lock_guard<std::mutex> lockDb(db.getMutex());
  return reservaRepo.obtenerPorSesion(idSesion);
}

bool DataManager::actualizarReserva(int id, const Reserva& reserva) {
  auto sesionMtx = obtenerMutexSesion(reserva.getIdSesion());
  std::lock_guard<std::mutex> lockSesion(*sesionMtx);
  std::lock_guard<std::mutex> lockDb(db.getMutex());
  return reservaRepo.actualizar(id, reserva);
}

bool DataManager::eliminarReserva(int id) {
  std::lock_guard<std::mutex> lockDb(db.getMutex());
  Reserva r = reservaRepo.obtenerPorId(id);
  if (r.getId() != -1) {
    auto sesionMtx = obtenerMutexSesion(r.getIdSesion());
    std::lock_guard<std::mutex> lockSesion(*sesionMtx);
    return reservaRepo.eliminar(id);
  }
  return false;
}

// --- Usuario ---
bool DataManager::crearUsuario(const Usuario& usuario) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return usuarioRepo.crear(usuario);
}

Usuario DataManager::obtenerUsuario(const std::string& dni) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return usuarioRepo.obtenerPorDni(dni);
}

Usuario DataManager::autenticarUsuario(const std::string& dni,
                                      const std::string& password) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return usuarioRepo.autenticar(dni, password);
}

bool DataManager::actualizarUsuario(const Usuario& usuario) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return usuarioRepo.actualizar(usuario);
}

bool DataManager::eliminarUsuario(const std::string& dni) {
  std::lock_guard<std::mutex> lock(db.getMutex());
  return usuarioRepo.eliminar(dni);
}