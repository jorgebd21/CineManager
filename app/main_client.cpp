#include <cstdio>
#include <iostream>
#include <iomanip>

#include "models/pelicula.hpp"
#include "models/asiento.hpp"
#include "models/reserva.hpp"
#include "models/sala.hpp"
#include "models/cine.hpp"
#include "UI/consola.hpp"
#include "db/datamanager.hpp"

using namespace std;

int main() {
    Consola consola;

    DataManager db;

    db.abrirSQL();

    int idCine = 1;
    Cine cine = db.obtenerCine(idCine);

    Reserva reserva_actual(-1, -1, -1, -1);
    int pelicula_id = -1;
    int sesion_id = -1;

    while(true) {
        consola.mostrarMenu();

        int opcion;

        cin >> opcion;
        cout << endl << "=======================================" << endl;
        switch(opcion) {
            case 1:
                for(auto& pelicula : db.obtenerCartelera(idCine)) {
                    consola.mostrarPelicula(pelicula);
                    cout << endl << "=======================================" << endl;
                }
                break;
            case 2: {
                for(auto& pelicula : db.obtenerCartelera(idCine)) {
                    consola.mostrarPelicula(pelicula);
                    cout << endl << "=======================================" << endl;
                }

                cout << "Seleccione el ID de una película: ";
                cin >> pelicula_id;

                auto sesiones = db.obtenerSesionesDePelicula(idCine, pelicula_id);
                if(sesiones.empty()) {
                    cout << "No se encontraron sesiones para esta película" << endl;
                    break;
                }
                for(auto& sesion : sesiones) {
                    cout << "=======================================" << endl;
                    cout << "ID Sesion: " << sesion.getId() << endl;
                    std::time_t hora = sesion.getHoraInicio();
                    cout << "Hora: " << std::put_time(std::localtime(&hora), "%Y-%m-%d %H:%M:%S") << endl;
                    cout << "Sala: " << sesion.getIdSala() << endl;
                    cout << "=======================================" << endl;
                }

                cout << "Seleccione el ID de una sesion: ";
                cin >> sesion_id;

                Sala sala_seleccionada = db.obtenerSala(db.obtenerSesion(sesion_id).getIdSala());
                if(sala_seleccionada.getId() == 0) {
                    cout << "Sala no encontrada" << endl;
                    break;
                }

                cout << endl << "=======================================" << endl;
                cout << "Sesion seleccionada: " << endl;
                auto reservas = db.obtenerReservasDeSesion(sesion_id);
                consola.mostrarSala(sala_seleccionada, reservas);
                cout << "Elija asiento (fila columna): " << endl;
                int fila, columna;
                cin >> fila >> columna;

                Reserva reserva_nueva(-1, sesion_id, fila-1, columna-1);
                if(reserva_nueva.getFila() < 0 || reserva_nueva.getColumna() < 0) {
                    cout << "Asiento invalido" << endl;
                    cout << endl << "=======================================" << endl;
                    break;
                }

                bool ya_ocupado = false;
                for (const auto& r : reservas) {
                    if (r.getFila() == reserva_nueva.getFila() && r.getColumna() == reserva_nueva.getColumna()) {
                        ya_ocupado = true;
                        break;
                    }
                }

                if(ya_ocupado){
                    cout << "Asiento ya seleccionado, elija otro" << endl;
                    cout << "=======================================" << endl;
                    break;
                }

                reserva_actual = reserva_nueva;
                cout << "Asiento seleccionado: " << fila << ", " << columna << endl;
                cout << "=======================================" << endl;
                break;
            }
            case 3:
                if(reserva_actual.getFila() == -1) {
                    cout << "Debe selecionar una sesión y un asiento para poder continuar" << endl;
                    break;
                }

                db.crearReserva(reserva_actual);

                cout << "Compra realizada con éxito" << endl;
                cout << "Película: " << pelicula_id << endl;
                cout << "Sala: " << sesion_id << endl;
                cout << "Asiento: " << reserva_actual.getFila()+1 << ", " << reserva_actual.getColumna()+1 << endl;
                break;
            case 4:
                cout << "Gracias por usar el sistema de gestión de cine" << endl;
                db.cerrarSQL();
                return 0;
            default:
                cout << "Opción no válida" << endl;
                cout << "Vuelva a intentarlo\n\n";
        }
    }

    return 0;
}