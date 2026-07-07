#include "UI/consola.hpp"
#include "models/sala.hpp"
#include "models/pelicula.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

void Consola::mostrarMenu(){
    cout << "=== CineManager ===" << endl;
    cout << "1. Cartelera" << endl;
    cout << "2. Elegir Pelicula" << endl;
    cout << "3. Comprar Entrada" << endl;
    cout << "4. Salir" << endl;
    cout << "Seleccione una opción: ";
}

void Consola::mostrarSala(const Sala& sala, const std::vector<Reserva>& reservas){
    cout << "=== Sala " << sala.getId() << " ===" << endl;
    cout << "Capacidad: " << sala.getCapacidad()-reservas.size() << endl;

    for (int i = 0; i < sala.getFilas(); i++){
        for (int j = 0 ; j < sala.getColumnas(); j++){
            if(reservas.end() != std::find_if(reservas.begin(), reservas.end(), [i, j](const Reserva& reserva){
                return reserva.getFila() == i && reserva.getColumna() == j;
            })){
                cout << "[X] ";
            } else{
                cout << "[-] ";
            }
        }
        cout << endl;
    }
}

void Consola::mostrarPelicula(const Pelicula& pelicula){
    cout << "=== Película " << pelicula.getId() << " ===" << endl;
    cout << "Título: " << pelicula.getTitulo() << endl;
    cout << "Duración: " << pelicula.getDuracion() << " minutos" << endl;
    cout << "Género: " << (int)pelicula.getGenero() << endl;
}