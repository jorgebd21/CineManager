#include "UI/consola.hpp"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "models/pelicula.hpp"
#include "models/sala.hpp"
#include "utils/time_utils.hpp"

using namespace std;

void Consola::mostrarMenu() {
  cout << "=== CineManager ===" << endl;
  cout << "1. Cartelera" << endl;
  cout << "2. Reservar Asiento" << endl;
  cout << "3. Comprar Entrada" << endl;
  cout << "4. Salir" << endl;
}

void Consola::mostrarSala(const Sala& sala,
                          const std::vector<Reserva>& reservas) {
  cout << "=== Sala " << sala.getId() << " ===" << endl;
  cout << "Capacidad: " << sala.getCapacidad() - reservas.size() << endl;

  for (int i = 0; i < sala.getFilas(); i++) {
    for (int j = 0; j < sala.getColumnas(); j++) {
      if (reservas.end() != std::find_if(reservas.begin(), reservas.end(),
                                         [i, j](const Reserva& reserva) {
                                           return reserva.getFila() == i &&
                                                  reserva.getColumna() == j;
                                         })) {
        cout << "[X] ";
      } else {
        cout << "[-] ";
      }
    }
    cout << endl;
  }
}

void Consola::mostrarPelicula(const Pelicula& pelicula) {
  cout << "=== Película " << pelicula.getId() << " ===" << endl;
  cout << "Título: " << pelicula.getTitulo() << endl;
  cout << "Duración: " << pelicula.getDuracion() << " minutos" << endl;
  cout << "Género: " << (int)pelicula.getGenero() << endl;
}

void Consola::mostrarCine(const Cine& cine) {
  cout << "=== Cine " << cine.getId() << " ===" << endl;
  cout << "ID: " << cine.getId() << endl;
  cout << "Nombre: " << cine.getNombre() << endl;
  cout << "Direccion: " << cine.getDireccion() << endl;
}
void Consola::mostrarSesion(const Sesion& sesion) {
  cout << "=== Sala " << sesion.getId() << " ===" << endl;
  cout << "ID: " << sesion.getId() << endl;
  cout << "Pelicula: " << sesion.getPelicula().getTitulo() << endl;
  cout << "Sala: " << sesion.getIdSala() << endl;
  std::tm tm_local = utils::safeLocalTime(sesion.getHoraInicio());
  cout << "Hora: " << std::put_time(&tm_local, "%Y-%m-%d %H:%M:%S")
       << endl;
}
void Consola::mostrarReporte(int totales, int ocupados, std::string_view titulo) {
  cout << endl << "=== Reporte de Ocupación " << titulo << " ===" << endl;
  cout << "Entradas Vendidas: " << ocupados << " / " << totales << endl;
  if (totales > 0) {
    double porcentaje = (ocupados * 100.0) / totales;
    cout << "Porcentaje de Ocupación: " << porcentaje << "%" << endl;
  } else {
    cout << "Este cine no tiene sesiones programadas." << endl;
  }
  cout << "==========================================" << endl;
}
void Consola::mostrarTicket(const Reserva& reserva, const Pelicula& pelicula,
                            const Sesion& sesion) {
  cout << endl;
  cout << "==========================================" << endl;
  cout << "            TICKET DE ENTRADA             " << endl;
  cout << "==========================================" << endl;
  cout << "  Película : " << pelicula.getTitulo() << endl;
  cout << "  Duración : " << pelicula.getDuracion() << " min" << endl;
  cout << "  Sala     : Sala " << sesion.getIdSala() << endl;
  cout << "  Asiento  : Fila " << reserva.getFila() + 1 << ", Butaca "
       << reserva.getColumna() + 1 << endl;
  std::tm tm_local = utils::safeLocalTime(sesion.getHoraInicio());
  cout << "  Horario  : "
       << std::put_time(&tm_local, "%Y-%m-%d %H:%M") << endl;
  cout << "  Estado   : " << reserva.getEstado() << endl;
  cout << "==========================================" << endl;
  cout << "  ¡Compra realizada con éxito! Disfrute de la función." << endl;
  cout << "==========================================" << endl
       << endl;
}

void Consola::mostrarMenuAdmin() {
  cout << "=== CineManager ADMIN ===" << endl;
  cout << "1. Gestion Cines" << endl;
  cout << "2. Gestion Peliculas" << endl;
  cout << "3. Gestion Salas" << endl;
  cout << "4. Gestion Sesiones" << endl;
  cout << "5. Reporte de Ocupacion" << endl;
  cout << "6. Salir" << endl;
  cout << "=========================" << endl;
}

void subMenuAdmin(std::string_view nombre) {
  cout << "=== Menu Gestion " << nombre << " ===" << endl;
  cout << "1. Listar" << endl;
  cout << "2. Añadir" << endl;
  cout << "3. Modificar" << endl;
  cout << "4. Eliminar" << endl;
  cout << "5. Volver" << endl;
  cout << "=========================" << endl;
}

void Consola::mostrarSubmenuCines() { subMenuAdmin("Cine"); }

void Consola::mostrarSubmenuPeliculas() { subMenuAdmin("Peliculas"); }

void Consola::mostrarSubmenuSalas() { subMenuAdmin("Salas"); }

void Consola::mostrarSubmenuSesiones() { subMenuAdmin("Seciones"); }

int Consola::pedirEntero(std::string_view mensaje) {
  int lectura;
  cout << mensaje << endl;
  while (true) {
    if (cin >> lectura) {
      cin.ignore(10000, '\n');
      return lectura;
    } else {
      cout << "Valor no valido, vuelva a introducirlo" << endl;
      cin.clear();
      cin.ignore(10000, '\n');
    }
  }
}

std::string Consola::pedirCadena(std::string_view mensaje) {
  std::string lectura;
  std::cout << mensaje;
  std::getline(std::cin, lectura);
  return lectura;
}