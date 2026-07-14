#include "controller/clientcontroller.hpp"

using namespace std;

void ClientController::ejecutar() {
  bool fin = false;
  int cineId = consola.pedirEntero("Introduzca la id del cine que desee: ");
  if (db.obtenerCine(cineId).getId() == -1) {
    cerr << "Id de cine invalido" << endl;
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
  if (reservaTemporal.getFila() < 0 || reservaTemporal.getColumna() < 0) {
    cout << "Asiento invalido" << endl;
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

  bool encontrado = false;
  Reserva reserva = db.obtenerReserva(reservaId);

  if (reserva.getId() == -1) {
    cerr << "Error por reserva invalida, compruebe si la ha realizado o si se "
            "le ha expirado"
         << endl;
    return false;
  } else {
    reserva.setEstado("COMPRADO");
    db.actualizarReserva(reserva.getId(), reserva);
    Sesion sesion = db.obtenerSesion(reserva.getIdSesion());
    consola.mostrarTicket(reserva, sesion.getPelicula(), sesion);
    return true;
  }
}