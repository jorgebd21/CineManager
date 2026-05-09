#include "UI/consola.hpp"
#include "models/sala.hpp"
#include "models/pelicula.hpp"
#include <iostream>

using namespace std;

void Consola::mostrarMenu(){
    cout << "=== Menú Principal ===" << endl;
    cout << "1. Gestionar Salas" << endl;
    cout << "2. Gestionar Películas" << endl;
    cout << "3. Salir" << endl;
    cout << "Seleccione una opción: ";
}

void Consola::mostrarSala(const Sala& sala){
    cout << "=== Sala " << sala.getId() << " ===" << endl;
    cout << "Capacidad: " << sala.getTotalAsientos() << endl;
    if(sala.getEstado() == OCUPADO){
        cout << "Estado: Ocupado" << endl;
        cout << "Película: " << sala.getPelicula().getTitulo() << endl;
        
        for(int i = 0; i < sala.getFilas(); i++){
            for(int j = 0; j < sala.getColumnas(); j++){
                 if(sala.getAsiento(i, j).getEstado() == OCUPADO){
                    cout << " [X] ";
                 } else {
                    cout << " [-] ";
                 }
            }
            cout << endl;
        }
    } else{
        cout << "Estado: Libre" << endl;
    }
}

void Consola::mostrarPelicula(const Pelicula& pelicula){
    cout << "=== Película " << pelicula.getId() << " ===" << endl;
    cout << "Título: " << pelicula.getTitulo() << endl;
    cout << "Duración: " << pelicula.getDuracion() << " minutos" << endl;
    cout << "Género: " << (int)pelicula.getGenero() << endl;
}