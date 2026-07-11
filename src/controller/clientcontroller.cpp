#include "controller/clientcontroller.hpp"

using namespace std;

void ClientController::ejecutar() {
  bool fin = false;
  int cineId = consola.pedirEntero("Introduzca la id del cine que desee: ");
  if (db.obtenerCine(cineId).getId() == -1) {
    cerr << "Id de cine invalido" << endl;
    return;
  }
  while (!fin) {
    consola.mostrarMenu();
    int seleccion = consola.pedirEntero("Seleccione una opción: ");
    switch (seleccion) {
      case 1:
        cartelera(cineId);
        break;
      case 2:
        reserva = realizarReserva(cineId);
        break;
      case 3:
        realizarCompra(cineId, reserva);
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

Reserva ClientController::realizarReserva(int cineId) {
  cartelera(cineId);
  int peliculaId = consola.pedirEntero("Seleccione el ID de una película: ");
  auto sesiones = db.obtenerSesionesDePelicula(cineId, peliculaId);
  if (sesiones.empty()) {
    cout << "No se encontraron sesiones para esta película" << endl;
    return Reserva(-1, -1, -1, -1);
  }
  for (auto& sesion : sesiones) {
    consola.mostrarSesion(sesion);
  }

  int sesionId = consola.pedirEntero("Seleccione el ID de una sesion: ");
  Sala sala = db.obtenerSala(db.obtenerSesion(sesionId).getIdSala());
  if (sala.getId() == 0) {
    cout << "Sala no encontrada" << endl;
    return Reserva(-1, -1, -1, -1);
  }

  cout << "Sesion seleccionada:" << endl;
  auto reservas = db.obtenerReservasDeSesion(sesionId);
  consola.mostrarSala(sala, reservas);
  int fila = consola.pedirEntero("Elija la fila: ");
  int columna = consola.pedirEntero("Elija la columna: ");

  Reserva reservaTemporal(-1, sesionId, fila - 1, columna - 1);
  if (reservaTemporal.getFila() < 0 || reservaTemporal.getColumna() < 0) {
    cout << "Asiento invalido" << endl;
    return Reserva(-1, -1, -1, -1);
  }

  bool ya_ocupado = false;
  for (const auto& r : reservas) {
    if (r.getFila() == reservaTemporal.getFila() &&
        r.getColumna() == reservaTemporal.getColumna()) {
      ya_ocupado = true;
      return Reserva(-1, -1, -1, -1);
    }
  }

  if (ya_ocupado) {
    cout << "Asiento invalido, ya esta reservado" << endl;
    return Reserva(-1, -1, -1, -1);
  }

  cout << "Reserva del asiento realizada con exito" << endl;
  return reservaTemporal;
}

bool ClientController::realizarCompra(int cineId, const Reserva& reserva) {
  if (reserva.getFila() < 0) {
    cout << "Debe selecionar una reserva para poder continuar" << endl;
    return false;
  }

  if (!db.crearReserva(reserva)) {
    cerr << "Error en la creacion de la reserva" << endl;
    return false;
  }

  Sesion sesion = db.obtenerSesion(reserva.getIdSesion());
  consola.mostrarTicket(reserva, sesion.getPelicula(), sesion);
  return true;
}