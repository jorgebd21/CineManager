#include "db/datamanager.hpp"

DataManager::~DataManager() {
  stopCleaner = true;
  cvCleaner.notify_all();
  if (cleanerThread.joinable()) {
    cleanerThread.join();
  }
}

std::mutex& DataManager::obtenerMutexSesion(int idSesion) {
  std::lock_guard<std::mutex> cerrojo(mapMutex);
  if (sessionMutexes.find(idSesion) == sessionMutexes.end()) {
    sessionMutexes[idSesion] = std::make_unique<std::mutex>();
  }
  return *sessionMutexes[idSesion];
}

std::vector<Reserva> DataManager::obtenerPendientes() {
  return reservaRepo.obtenerPendientes();
}

void DataManager::iniciarLimpiezaLoop() {
  while (!stopCleaner) {
    std::unique_lock<std::mutex> lock(cvMutex);
    cvCleaner.wait_for(lock, std::chrono::seconds(5),
                       [this]() { return stopCleaner.load(); });

    if (stopCleaner == true) {
      break;
    }

    std::time_t ahora = std::time(nullptr);
    std::vector<Reserva> reservas = obtenerPendientes();
    for (auto reserva : reservas) {
      if (TIEMPO_EXPIRACION_SEGUNDOS <=
          ahora - reserva.getTimestampCreacion()) {
        eliminarReserva(reserva.getId());
      }
    }
  }
}

// --- Cine ---
int DataManager::crearCine(const Cine& cine) { return cineRepo.crear(cine); }
Cine DataManager::obtenerCine(int id) { return cineRepo.obtenerPorId(id); }
std::vector<Cine> DataManager::obtenerCines() {
  return cineRepo.obtenerTodos();
}
bool DataManager::actualizarCine(int id, const Cine& cine) {
  return cineRepo.actualizar(id, cine);
}
bool DataManager::eliminarCine(int id) { return cineRepo.eliminar(id); }

// --- Sala ---
int DataManager::crearSala(const Sala& sala) { return salaRepo.crear(sala); }
Sala DataManager::obtenerSala(int id) { return salaRepo.obtenerPorId(id); }
std::vector<Sala> DataManager::obtenerSalas() {
  return salaRepo.obtenerTodos();
}
std::vector<Sala> DataManager::obtenerSalasDeCine(int idCine) {
  return salaRepo.obtenerSalasDeCine(idCine);
}
bool DataManager::actualizarSala(int id, const Sala& sala) {
  return salaRepo.actualizar(id, sala);
}
bool DataManager::eliminarSala(int id) { return salaRepo.eliminar(id); }

// --- Pelicula ---
int DataManager::crearPelicula(const Pelicula& pelicula) {
  return peliculaRepo.crear(pelicula);
}
Pelicula DataManager::obtenerPelicula(int id) {
  return peliculaRepo.obtenerPorId(id);
}
std::vector<Pelicula> DataManager::obtenerPeliculas() {
  return peliculaRepo.obtenerTodos();
}
std::vector<Pelicula> DataManager::obtenerCartelera(int idCine) {
  return peliculaRepo.obtenerCartelera(idCine);
}
bool DataManager::actualizarPelicula(int id, const Pelicula& pelicula) {
  return peliculaRepo.actualizar(id, pelicula);
}
bool DataManager::eliminarPelicula(int id) { return peliculaRepo.eliminar(id); }

// --- Sesion ---
int DataManager::crearSesion(const Sesion& sesion) {
  return sesionRepo.crear(sesion);
}
Sesion DataManager::obtenerSesion(int id) {
  return sesionRepo.obtenerPorId(id);
}
std::vector<Sesion> DataManager::obtenerSesiones() {
  return sesionRepo.obtenerTodos();
}
std::vector<Sesion> DataManager::obtenerSesionesDeCine(int idCine) {
  return sesionRepo.obtenerSesionesDeCine(idCine);
}
std::vector<Sesion> DataManager::obtenerSesionesDePelicula(int idCine,
                                                           int idPelicula) {
  return sesionRepo.obtenerSesionesDePelicula(idCine, idPelicula);
}
bool DataManager::actualizarSesion(int id, const Sesion& sesion) {
  return sesionRepo.actualizar(id, sesion);
}
bool DataManager::eliminarSesion(int id) { return sesionRepo.eliminar(id); }

// --- Reserva ---
int DataManager::crearReserva(const Reserva& reserva) {
  std::lock_guard<std::mutex> candado(
      obtenerMutexSesion(reserva.getIdSesion()));
  bool encontrado = false;
  for (auto reservaLista : obtenerReservasDeSesion(reserva.getIdSesion())) {
    if (reservaLista.getIdSesion() == reserva.getIdSesion() &&
        reservaLista.getFila() == reserva.getFila() &&
        reservaLista.getColumna() == reserva.getColumna()) {
      encontrado = true;
    }
  }
  if (encontrado) {
    return -1;
  } else {
    return reservaRepo.crear(reserva);
  }
}
Reserva DataManager::obtenerReserva(int id) {
  return reservaRepo.obtenerPorId(id);
}
std::vector<Reserva> DataManager::obtenerReservasDeSesion(int idSesion) {
  return reservaRepo.obtenerPorSesion(idSesion);
}
bool DataManager::actualizarReserva(int id, const Reserva& reserva) {
  std::lock_guard<std::mutex> candado(
      obtenerMutexSesion(reserva.getIdSesion()));
  return reservaRepo.actualizar(id, reserva);
}
bool DataManager::eliminarReserva(int id) {
  Reserva r = obtenerReserva(id);
  if (r.getId() != -1) {
    std::lock_guard<std::mutex> candado(obtenerMutexSesion(r.getIdSesion()));
    return reservaRepo.eliminar(id);
  }
  return false;
}

bool DataManager::crearUsuario(const Usuario& usuario) {
  return usuarioRepo.crear(usuario);
}

Usuario DataManager::obtenerUsuario(const std::string& dni) {
  return usuarioRepo.obtenerPorDni(dni);
}

Usuario DataManager::autenticarUsuario(const std::string& dni,
                                      const std::string& password) {
  return usuarioRepo.autenticar(dni, password);
}

bool DataManager::actualizarUsuario(const Usuario& usuario) {
  return usuarioRepo.actualizar(usuario);
}

bool DataManager::eliminarUsuario(const std::string& dni) {
  return usuarioRepo.eliminar(dni);
}