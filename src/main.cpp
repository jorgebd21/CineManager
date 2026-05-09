#include <iostream>
#include "models/pelicula.hpp"
#include "models/asiento.hpp"
#include "models/sala.hpp"
#include "models/cine.hpp"
#include "UI/consola.hpp"

using namespace std;

int main() {
    Consola consola;

    // Test: Mostrar una película
    cout << "\n=== TEST: Mostrar Película ===" << endl;
    Pelicula pelicula1("Avatar 2", Genero::CIENCIA_FICCION, 192);
    Pelicula pelicula2("Pulp Fiction", Genero::DRAMA, 154);

    consola.mostrarPelicula(pelicula1);
    cout << endl;
    consola.mostrarPelicula(pelicula2);

    // Test: Mostrar sala libre
    cout << "\n=== TEST: Mostrar Sala Libre ===" << endl;
    Sala sala1(5, 6);
    consola.mostrarSala(sala1);

    // Test: Mostrar sala ocupada con película y asientos reservados
    cout << "\n=== TEST: Mostrar Sala Ocupada ===" << endl;
    Sala sala2(5, 6);
    sala2.reservarSala(pelicula1);
    sala2.reservarAsiento(0, 0);
    sala2.reservarAsiento(0, 1);
    sala2.reservarAsiento(1, 3);
    sala2.reservarAsiento(2, 4);
    sala2.reservarAsiento(4, 5);

    consola.mostrarSala(sala2);

    return 0;
}