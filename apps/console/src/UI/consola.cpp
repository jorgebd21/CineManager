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

void Consola::mostrarMenu() {
  std::cout << "=== CineManager ===" << std::endl;
  std::cout << "1. Cartelera" << std::endl;
  std::cout << "2. Reservar Asiento" << std::endl;
  std::cout << "3. Comprar Entrada" << std::endl;
  std::cout << "4. Salir" << std::endl;
}

void Consola::mostrarSala(const Sala& sala,
                          const std::vector<Reserva>& reservas) {
  std::cout << "=== Sala " << sala.getId() << " ===" << std::endl;
  std::cout << "Capacidad: " << sala.getCapacidad() - reservas.size() << std::endl;

  for (int i = 0; i < sala.getFilas(); i++) {
    for (int j = 0; j < sala.getColumnas(); j++) {
      if (reservas.end() != std::find_if(reservas.begin(), reservas.end(),
                                         [i, j](const Reserva& reserva) {
                                           return reserva.getFila() == i &&
                                                  reserva.getColumna() == j;
                                         })) {
        std::cout << "[X] ";
      } else {
        std::cout << "[-] ";
      }
    }
    std::cout << std::endl;
  }
}

void Consola::mostrarPelicula(const Pelicula& pelicula) {
  std::cout << "=== Película " << pelicula.getId() << " ===" << std::endl;
  std::cout << "Título: " << pelicula.getTitulo() << std::endl;
  std::cout << "Duración: " << pelicula.getDuracion() << " minutos" << std::endl;
  std::cout << "Género: " << (int)pelicula.getGenero() << std::endl;
}

void Consola::mostrarCine(const Cine& cine) {
  std::cout << "=== Cine " << cine.getId() << " ===" << std::endl;
  std::cout << "ID: " << cine.getId() << std::endl;
  std::cout << "Nombre: " << cine.getNombre() << std::endl;
  std::cout << "Direccion: " << cine.getDireccion() << std::endl;
}
void Consola::mostrarSesion(const Sesion& sesion) {
  std::cout << "=== Sala " << sesion.getId() << " ===" << std::endl;
  std::cout << "ID: " << sesion.getId() << std::endl;
  std::cout << "Pelicula: " << sesion.getPelicula().getTitulo() << std::endl;
  std::cout << "Sala: " << sesion.getIdSala() << std::endl;
  std::tm tm_local = utils::safeLocalTime(sesion.getHoraInicio());
  std::cout << "Hora: " << std::put_time(&tm_local, "%Y-%m-%d %H:%M:%S")
            << std::endl;
}
void Consola::mostrarReporte(int totales, int ocupados, std::string_view titulo) {
  std::cout << std::endl << "=== Reporte de Ocupación " << titulo << " ===" << std::endl;
  std::cout << "Entradas Vendidas: " << ocupados << " / " << totales << std::endl;
  if (totales > 0) {
    double porcentaje = (ocupados * 100.0) / totales;
    std::cout << "Porcentaje de Ocupación: " << porcentaje << "%" << std::endl;
  } else {
    std::cout << "Este cine no tiene sesiones programadas." << std::endl;
  }
  std::cout << "==========================================" << std::endl;
}
void Consola::mostrarTicket(const Reserva& reserva, const Pelicula& pelicula,
                            const Sesion& sesion) {
  std::cout << std::endl;
  std::cout << "==========================================" << std::endl;
  std::cout << "            TICKET DE ENTRADA             " << std::endl;
  std::cout << "==========================================" << std::endl;
  std::cout << "  Película : " << pelicula.getTitulo() << std::endl;
  std::cout << "  Duración : " << pelicula.getDuracion() << " min" << std::endl;
  std::cout << "  Sala     : Sala " << sesion.getIdSala() << std::endl;
  std::cout << "  Asiento  : Fila " << reserva.getFila() + 1 << ", Butaca "
            << reserva.getColumna() + 1 << std::endl;
  std::tm tm_local = utils::safeLocalTime(sesion.getHoraInicio());
  std::cout << "  Horario  : "
            << std::put_time(&tm_local, "%Y-%m-%d %H:%M") << std::endl;
  std::cout << "  Estado   : " << reserva.getEstado() << std::endl;
  std::cout << "==========================================" << std::endl;
  std::cout << "  ¡Compra realizada con éxito! Disfrute de la función." << std::endl;
  std::cout << "==========================================" << std::endl
            << std::endl;
}

void Consola::mostrarMenuAdmin() {
  std::cout << "=== CineManager ADMIN ===" << std::endl;
  std::cout << "1. Gestion Cines" << std::endl;
  std::cout << "2. Gestion Peliculas" << std::endl;
  std::cout << "3. Gestion Salas" << std::endl;
  std::cout << "4. Gestion Sesiones" << std::endl;
  std::cout << "5. Reporte de Ocupacion" << std::endl;
  std::cout << "6. Salir" << std::endl;
  std::cout << "=========================" << std::endl;
}

void subMenuAdmin(std::string_view nombre) {
  std::cout << "=== Menu Gestion " << nombre << " ===" << std::endl;
  std::cout << "1. Listar" << std::endl;
  std::cout << "2. Añadir" << std::endl;
  std::cout << "3. Modificar" << std::endl;
  std::cout << "4. Eliminar" << std::endl;
  std::cout << "5. Volver" << std::endl;
  std::cout << "=========================" << std::endl;
}

void Consola::mostrarSubmenuCines() { subMenuAdmin("Cine"); }

void Consola::mostrarSubmenuPeliculas() { subMenuAdmin("Peliculas"); }

void Consola::mostrarSubmenuSalas() { subMenuAdmin("Salas"); }

void Consola::mostrarSubmenuSesiones() { subMenuAdmin("Sesiones"); }

int Consola::pedirEntero(std::string_view mensaje) {
  int lectura;
  std::cout << mensaje << std::endl;
  while (true) {
    if (std::cin >> lectura) {
      std::cin.ignore(10000, '\n');
      return lectura;
    } else {
      std::cout << "Valor no valido, vuelva a introducirlo" << std::endl;
      std::cin.clear();
      std::cin.ignore(10000, '\n');
    }
  }
}

std::string Consola::pedirCadena(std::string_view mensaje) {
  std::string lectura;
  std::cout << mensaje;
  std::getline(std::cin, lectura);
  return lectura;
}