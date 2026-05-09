#include <iostream>
#include "models/pelicula.hpp"
#include "models/asiento.hpp"
#include "models/sala.hpp"
#include "models/cine.hpp"
#include "UI/consola.hpp"

using namespace std;

int main() {
    Consola consola;

    Cine cine;
    
    cine.agregarPelicula(Pelicula("Avatar 2", Genero::CIENCIA_FICCION, 192));
    cine.agregarPelicula(Pelicula("Pulp Fiction", Genero::DRAMA, 154));
    cine.agregarSala(Sala(5,6));
    cine.getSalas()[0].reservarSala(cine.getPeliculas()[0]);

    auto salas = cine.getSalas();
    auto peliculas = cine.getPeliculas();
    Asiento asiento_seleccionado = Asiento();

    Sala *sala_seleccionada = nullptr;
    Pelicula *pelicula_seleccionada = nullptr;

    while(true) {
        consola.mostrarMenu();

        int opcion;

        cin >> opcion;
        cout << endl << "=======================================" << endl;
        switch(opcion) {
            case 1:
                for(auto& pelicula : peliculas) {
                    consola.mostrarPelicula(pelicula);
                    cout << endl << "=======================================" << endl;
                }
                break;
            case 2: {
                for(const auto& sala : cine.getSalasOcupadas()) {
                    consola.mostrarPelicula(sala.getPelicula());
                    consola.mostrarSala(sala);
                    cout << endl;
                    cout << endl << "=======================================" << endl;
                }

                cout << "Seleccione una película: ";
                int pelicula_id;
                cin >> pelicula_id;

                for(auto& sala : cine.getSalas()) {
                    if(sala.estaOcupada() && sala.getPelicula().getId() == pelicula_id) {
                        cout << "Película seleccionada: " << endl;
                        consola.mostrarPelicula(sala.getPelicula());
                        sala_seleccionada = &sala;
                        pelicula_seleccionada = &sala.getPelicula();
                        break;
                    }
                }

                if(sala_seleccionada == nullptr) {
                    cout << "Película no encontrada" << endl;
                    break;
                }

                cout << endl << "=======================================" << endl;
                cout << "Sala seleccionada: " << endl;
                consola.mostrarSala(*sala_seleccionada);
                cout << "Elija asiento (fila columna): " << endl;
                int fila, columna;
                cin >> fila >> columna;

                Asiento& asiento = sala_seleccionada->getAsiento(fila-1, columna-1);
                if(asiento.getFila() == -1) {
                    cout << "Asiento invalido" << endl;
                    cout << endl << "=======================================" << endl;
                    break;
                }
                if(asiento.estaOcupado()) {
                    cout << "Asiento ocupado" << endl;
                    cout << endl << "=======================================" << endl;
                    break;
                }

                sala_seleccionada->reservarAsiento(fila, columna);
                asiento_seleccionado = asiento;
                cout << "Asiento seleccionado: " << fila << ", " << columna << endl;
                cout << endl << "=======================================" << endl;
                break;
            }
            case 3:
                if(pelicula_seleccionada == nullptr) {
                    cout << "Debe seleccionar una película primero" << endl;
                    break;
                }else if(asiento_seleccionado.getFila() == -1) {
                    cout << "Debe seleccionar un asiento primero" << endl;
                    break;
                }

                cout << "Compra realizada con éxito" << endl;
                cout << "Película: " << pelicula_seleccionada->getTitulo() << endl;
                cout << "Sala: " << sala_seleccionada->getId() << endl;
                cout << "Asiento: " << asiento_seleccionado.getFila()+1 << ", " << asiento_seleccionado.getColumna()+1 << endl;

            case 4:
                cout << "Gracias por usar el sistema de gestión de cine" << endl;
                return 0;
            default:
                cout << "Opción no válida" << endl;
                cout << "Vuelva a intentarlo\n\n";
        }
    }

    return 0;
}