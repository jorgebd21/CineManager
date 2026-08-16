#include "controller/clientcontroller.hpp"

using namespace std;

bool ClientController::iniciarSesion() {
  cout << "========================================" << endl;
  cout << "       ACCESO CLIENTE - CINEMANAGER     " << endl;
  cout << "========================================" << endl;

  constexpr int MAX_INTENTOS = 3;
  for (int intento = 1; intento <= MAX_INTENTOS; ++intento) {
    string dni = consola.pedirCadena("Introduzca su DNI: ");
    string password = consola.pedirCadena("Introduzca su contraseña: ");

    Usuario user = db.autenticarUsuario(dni, password);
    if (user.esValido()) {
      usuarioActual = user;
      cout << "\n✅ Autenticación exitosa. ¡Bienvenido/a, "
           << usuarioActual.getNombre() << " " << usuarioActual.getApellidos()
           << "!\n"
           << endl;
      return true;
    }

    cerr << "\n❌ Credenciales incorrectas. Intento " << intento << " de "
         << MAX_INTENTOS << ".\n"
         << endl;
  }
  cerr << "Demasiados intentos fallidos. Cerrando aplicación." << endl;
  return false;
}

void ClientController::ejecutar() {
  if (!iniciarSesion()) {
    return;
  }

  // Mostrar listado de cines disponibles antes de pedir selección
  auto cines = db.obtenerCines();
  if (cines.empty()) {
    cerr << "No hay cines disponibles en el sistema." << endl;
    return;
  }
  cout << endl;
  cout << "========================================" << endl;
  cout << "        CINES DISPONIBLES               " << endl;
  cout << "========================================" << endl;
  for (const auto& cine : cines) {
    cout << "  [" << cine.getId() << "] " << cine.getNombre() << endl;
    cout << "       " << cine.getDireccion() << endl;
  }
  cout << "========================================" << endl;
  cout << endl;

  bool fin = false;
  int cineId = consola.pedirEntero("Introduzca el ID del cine que desee: ");
  if (db.obtenerCine(cineId).getId() == -1) {
    cerr << "ID de cine invalido. Compruebe el listado anterior." << endl;
    return;
  }
  int id = -1;
  while (!fin) {
    consola.mostrarMenu();
    int seleccion = consola.pedirEntero("Seleccione una opción: ");
    switch (seleccion) {
      case 1:
        cartelera(cineId);
        break;
      case 2:
        id = realizarReserva(cineId);
        break;
      case 3:
        realizarCompra(cineId, id);
        break;
      case 4:
        cout << "Gracias por usar el sistema administrador de gestión de cine"
             << endl;
        fin = true;
        break;
      default:
        cout << "Opción no válida" << endl << "Vuelva a intentarlo";
    }
  }
}

void ClientController::cartelera(int cineId) {
  for (auto& pelicula : db.obtenerCartelera(cineId)) {
    consola.mostrarPelicula(pelicula);
  }
}

int ClientController::realizarReserva(int cineId) {
  cartelera(cineId);
  int peliculaId = consola.pedirEntero("Seleccione el ID de una película: ");
  auto sesiones = db.obtenerSesionesDePelicula(cineId, peliculaId);
  if (sesiones.empty()) {
    cout << "No se encontraron sesiones para esta película" << endl;
    return -1;
  }
  for (auto& sesion : sesiones) {
    consola.mostrarSesion(sesion);
  }

  int sesionId = consola.pedirEntero("Seleccione el ID de una sesion: ");
  Sala sala = db.obtenerSala(db.obtenerSesion(sesionId).getIdSala());
  if (sala.getId() == 0) {
    cout << "Sala no encontrada" << endl;
    return -1;
  }

  cout << "Sesion seleccionada:" << endl;
  auto reservas = db.obtenerReservasDeSesion(sesionId);
  consola.mostrarSala(sala, reservas);
  int fila = consola.pedirEntero("Elija la fila: ");
  int columna = consola.pedirEntero("Elija la columna: ");

  Reserva reservaTemporal(-1, sesionId, fila - 1, columna - 1, "PENDIENTE",
                          std::time(nullptr));
  if (reservaTemporal.getFila() < 0 ||
      reservaTemporal.getFila() >= sala.getFilas() ||
      reservaTemporal.getColumna() < 0 ||
      reservaTemporal.getColumna() >= sala.getColumnas()) {
    cout << "Asiento invalido (coordenadas fuera de los limites de la sala)" << endl;
    return -1;
  }

  bool ya_ocupado = false;
  for (const auto& r : reservas) {
    if (r.getFila() == reservaTemporal.getFila() &&
        r.getColumna() == reservaTemporal.getColumna()) {
      ya_ocupado = true;
      break;
    }
  }

  if (ya_ocupado) {
    cout << "Asiento invalido, ya esta reservado" << endl;
    return -1;
  }

  int id = db.crearReserva(reservaTemporal);
  if (id != -1) {
    cout << "Reserva del asiento realizada con exito" << endl;
    return id;
  } else {
    cerr << "Error en al creacion de la Reserva en el sistema" << endl;
    return -1;
  }
}

bool ClientController::realizarCompra(int cineId, int reservaId) {
  if (reservaId == -1) {
    cout << "Debe selecionar una reserva para poder continuar" << endl;
    return false;
  }

  Reserva reserva = db.obtenerReserva(reservaId);

  if (reserva.getId() == -1) {
    cerr << "Error por reserva invalida, compruebe si la ha realizado o si se "
            "le ha expirado"
         << endl;
    return false;
  }

  reserva.setEstado("COMPRADO");
  if (!db.actualizarReserva(reserva.getId(), reserva)) {
    cerr << "Error al confirmar la compra en la base de datos." << endl;
    return false;
  }

  Sesion sesion = db.obtenerSesion(reserva.getIdSesion());
  consola.mostrarTicket(reserva, sesion.getPelicula(), sesion);
  return true;
}