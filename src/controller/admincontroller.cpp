#include "controller/admincontroller.hpp"

#include <endian.h>

#include <iostream>

#include "models/pelicula.hpp"

using namespace std;

AdminController::AdminController(DataManager& database, Consola& ui)
    : db(database), consola(ui) {}

void AdminController::ejecutar() {
  bool fin = false;
  while (!fin) {
    consola.mostrarMenuAdmin();

    int seleccion = consola.pedirEntero("Seleccione una opción: ");
    switch (seleccion) {
      case 1:
        gestionarCines();
        break;
      case 2:
        gestionarPeliculas();
        break;
      case 3:
        gestionarSalas();
        break;
      case 4:
        gestionarSesiones();
        break;
      case 5: {
        mostrarReporteOcupacion();
        break;
      }
      case 6:
        cout << "Gracias por usar el sistema administrador de gestión de cine"
             << endl;
        fin = true;
        break;
      default:
        cout << "Opción no válida" << endl << "Vuelva a intentarlo";
    }
  }
}

void AdminController::gestionarCines() {
  bool fin = false;
  while (!fin) {
    consola.mostrarSubmenuCines();
    int opcion =
        consola.pedirEntero("Seleccione la opcion que de desea realizar: ");
    switch (opcion) {
      case 1:
        for (auto cine : db.obtenerCines()) {
          consola.mostrarCine(cine);
        }
        break;
      case 2: {
        string nombre = consola.pedirCadena("Introduzca el nombre del cine: ");
        string direccion =
            consola.pedirCadena("Introduzca la direccion del cine: ");

        Cine cine(-1, nombre, direccion);

        if (db.crearCine(cine)) {
          cout << "Se ha creado exitosamente" << endl;
        } else {
          cerr << "Fallo en la creacion, vuelvalo a intentar" << endl;
        }
        break;
      }
      case 3: {
        int id = consola.pedirEntero(
            "Introduzca la id del objeto que quiere modificar: ");
        Cine cine = db.obtenerCine(id);
        if (cine.getId() == -1) {
          cerr << "La id no es valida" << endl;
          break;
        }

        bool salida = false;
        while (!salida) {
          int opcion = consola.pedirEntero(
              "Eliga que valor desea modificar (1-nombre, 2-direccion)");
          string nuevoValor =
              consola.pedirCadena("Introduzca el nuevo valor: ");
          switch (opcion) {
            case 1:
              cine.setNombre(nuevoValor);
              break;
            case 2:
              cine.setDireccion(nuevoValor);
              break;
            default:
              cout << "Opción no válida" << endl << "Vuelva a intentarlo";
          }

          if (db.actualizarCine(id, cine)) {
            salida = true;
          }
        }
        break;
      }
      case 4: {
        bool salida = false;
        while (!salida) {
          int id = consola.pedirEntero(
              "Introduzca la id del objeto que desea eliminar: ");
          Cine cine = db.obtenerCine(id);
          if (cine.getId() == -1) {
            cerr << "La id no es valida" << endl;
            break;
          }

          consola.mostrarCine(cine);
          string confirmacion = consola.pedirCadena(
              "Para confirmar su eliminacion, escriba el nombre del cine: ");
          if (confirmacion == cine.getNombre()) {
            cout << "Eliminando objeto" << endl;
            if (db.eliminarCine(id)) {
              cout << "Objeto eliminado" << endl;
            } else {
              cerr << "Error al eliminar el objeto" << endl;
            }
          }
          salida = true;
        }
        break;
      }
      case 5:
        fin = true;
        break;
    }
  }
}

void AdminController::gestionarPeliculas() {
  bool fin = false;
  while (!fin) {
    consola.mostrarSubmenuPeliculas();
    int opcion =
        consola.pedirEntero("Seleccione la opcion que de desea realizar: ");
    switch (opcion) {
      case 1:
        for (auto pelicula : db.obtenerPeliculas()) {
          consola.mostrarPelicula(pelicula);
        }
        break;
      case 2: {
        string titulo = consola.pedirCadena("Introduzca el titulo: ");
        int duracion = consola.pedirEntero("Introduzca la duraccion: ");
        cout << "1-ACCION\n2-COMEDIA\n3-DRAMA\n4-TERROR\n5-CIENCIA_"
                "FICCION\n6-ROMANCE\n7-DOCUMENTAL\n8-OTHER"
             << endl;
        int generoN = consola.pedirEntero("Introduzca el numero del genero: ");

        Pelicula pelicula(-1, titulo, intToGenero(generoN + 1), duracion);

        if (db.crearPelicula(pelicula)) {
          cout << "Se ha creado exitosamente" << endl;
        } else {
          cerr << "Fallo en la creacion, vuelvalo a intentar" << endl;
        }
        break;
      }
      case 3: {
        int id = consola.pedirEntero(
            "Introduzca la id del objeto que quiere modificar: ");
        Pelicula pelicula = db.obtenerPelicula(id);
        if (pelicula.getId() == -1) {
          cerr << "La id no es valida" << endl;
          break;
        }

        bool salida = false;
        while (!salida) {
          int opcion = consola.pedirEntero(
              "Eliga que valor desea modificar (1-titulo, 2-duracion, "
              "3-genero)");
          switch (opcion) {
            case 1: {
              string titulo =
                  consola.pedirCadena("Introduzca el nuevo valor: ");
              pelicula.setTitulo(titulo);
              break;
            }
            case 2: {
              int duracion = consola.pedirEntero("Introduzca el nuevo valor: ");
              pelicula.setDuracion(duracion);
              break;
            }
            case 3: {
              cout << "1-ACCION\n2-COMEDIA\n3-DRAMA\n4-TERROR\n5-CIENCIA_"
                      "FICCION\n6-ROMANCE\n7-DOCUMENTAL\n8-OTHER"
                   << endl;
              int generoN = consola.pedirEntero("Introduzca el nuevo valor: ");
              pelicula.setGenero(intToGenero(generoN + 1));
              break;
            }
            default:
              cout << "Opción no válida" << endl << "Vuelva a intentarlo";
          }

          if (db.actualizarPelicula(id, pelicula)) {
            salida = true;
          }
        }
        break;
      }
      case 4: {
        bool salida = false;
        while (!salida) {
          int id = consola.pedirEntero(
              "Introduzca la id del objeto que desea eliminar: ");
          Pelicula pelicula = db.obtenerPelicula(id);
          if (pelicula.getId() == -1) {
            cerr << "La id no es valida" << endl;
            break;
          }

          consola.mostrarPelicula(pelicula);
          string confirmacion = consola.pedirCadena(
              "Para confirmar su eliminacion, escriba el titulo de la "
              "pelicula: ");
          if (confirmacion == pelicula.getTitulo()) {
            cout << "Eliminando objeto" << endl;
            if (db.eliminarPelicula(id)) {
              cout << "Objeto eliminado" << endl;
            } else {
              cerr << "Error al eliminar el objeto" << endl;
            }
          }
          salida = true;
        }
        break;
      }
      case 5:
        fin = true;
        break;
    }
  }
}

void AdminController::gestionarSalas() {
  bool fin = false;
  while (!fin) {
    consola.mostrarSubmenuSalas();
    int opcion =
        consola.pedirEntero("Seleccione la opcion que de desea realizar: ");
    switch (opcion) {
      case 1:
        for (auto sala : db.obtenerSalas()) {
          consola.mostrarSala(sala);
        }
        break;
      case 2: {
        int cineId = consola.pedirEntero("Introduzca el id del cine: ");
        int numeroSala =
            consola.pedirEntero("Introduzca el numero de la sala: ");
        int filas = consola.pedirEntero("Introduzca el numero de filas: ");
        int columnas =
            consola.pedirEntero("Introduzca el numero de columnas: ");

        Sala sala(-1, cineId, numeroSala, filas, columnas);

        if (db.crearSala(sala)) {
          cout << "Se ha creado exitosamente" << endl;
        } else {
          cerr << "Fallo en la creacion, vuelvalo a intentar" << endl;
        }
        break;
      }
      case 3: {
        int id = consola.pedirEntero(
            "Introduzca la id del objeto que quiere modificar: ");
        Sala sala = db.obtenerSala(id);
        if (sala.getId() == -1) {
          cerr << "La id no es valida" << endl;
          break;
        }

        bool salida = false;
        while (!salida) {
          int opcion = consola.pedirEntero(
              "Eliga que valor desea modificar (1-NumeroSala, 2-Filas, "
              "3-Columnas)");
          int nuevoValor = consola.pedirEntero("Introduzca el nuevo valor: ");
          switch (opcion) {
            case 1: {
              sala.setNumeroSala(nuevoValor);
              break;
            }
            case 2: {
              sala.setFilas(nuevoValor);
              break;
            }
            case 3: {
              sala.setColumnas(nuevoValor);
              break;
            }
            default:
              cout << "Opción no válida" << endl << "Vuelva a intentarlo";
          }

          if (db.actualizarSala(id, sala)) {
            salida = true;
          }
        }
        break;
      }
      case 4: {
        bool salida = false;
        while (!salida) {
          int id = consola.pedirEntero(
              "Introduzca la id del objeto que desea eliminar: ");
          Sala sala = db.obtenerSala(id);
          if (sala.getId() == -1) {
            cerr << "La id no es valida" << endl;
            break;
          }

          consola.mostrarSala(sala);
          string confirmacion = consola.pedirCadena(
              "Para confirmar su eliminacion, escriba el numero de la sala con "
              "el siguiente formato (SALA x): ");
          if (confirmacion == ("SALA " + to_string(sala.getNumeroSala()))) {
            cout << "Eliminando objeto" << endl;
            if (db.eliminarSala(id)) {
              cout << "Objeto eliminado" << endl;
            } else {
              cerr << "Error al eliminar el objeto" << endl;
            }
          }
          salida = true;
        }
        break;
      }
      case 5:
        fin = true;
        break;
    }
  }
}

void AdminController::gestionarSesiones() {
  bool fin = false;
  while (!fin) {
    consola.mostrarSubmenuSesiones();
    int opcion =
        consola.pedirEntero("Seleccione la opcion que de desea realizar: ");
    switch (opcion) {
      case 1:
        for (auto sesion : db.obtenerSesiones()) {
          consola.mostrarSesion(sesion);
        }
        break;
      case 2: {
        int peliculaId =
            consola.pedirEntero("Introduzca la id de la pelicula: ");
        Pelicula pelicula = db.obtenerPelicula(peliculaId);
        if (pelicula.getId() == -1) {
          cerr << "La id de la pelicula no es valida" << endl;
          break;
        }

        int salaId = consola.pedirEntero("Introduzca el id de la sala: ");
        string fechaTexto =
            consola.pedirCadena("Introduzca la fecha (YYYY-MM-DD HH:MM:SS): ");
        std::tm tm_time = {};
        std::istringstream ss(fechaTexto);
        ss >> std::get_time(&tm_time, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
          cout << "Formato de fecha incorrecto." << endl;
          break;
        }
        std::time_t timestamp = std::mktime(&tm_time);

        Sesion sesion(-1, pelicula, salaId, timestamp);

        if (db.crearSesion(sesion)) {
          cout << "Se ha creado exitosamente" << endl;
        } else {
          cerr << "Fallo en la creacion, vuelvalo a intentar" << endl;
        }
        break;
      }
      case 3: {
        int id = consola.pedirEntero(
            "Introduzca la id del objeto que quiere modificar: ");
        Sesion sesion = db.obtenerSesion(id);
        if (sesion.getId() == -1) {
          cerr << "La id no es valida" << endl;
          break;
        }

        bool salida = false;
        while (!salida) {
          int opcion = consola.pedirEntero(
              "Eliga que valor desea modificar (1-Pelicula, 2-Sala, "
              "3-Fecha/Hora)");
          switch (opcion) {
            case 1: {
              int peliculaId =
                  consola.pedirEntero("Introduzca el nuevo valor: ");
              Pelicula pelicula = db.obtenerPelicula(peliculaId);
              if (pelicula.getId() != -1) {
                sesion.setPelicula(pelicula);
              } else {
                cout << "Película no encontrada." << endl;
              }
              break;
            }
            case 2: {
              int salaId = consola.pedirEntero("Introduzca el nuevo valor: ");
              Sala s = db.obtenerSala(salaId);
              if (s.getId() != -1) {
                sesion.setIdSala(salaId);
              } else {
                cout << "Sala no encontrada." << endl;
              }
              break;
            }
            case 3: {
              string fechaTexto =
                  consola.pedirCadena("Introduzca el nuevo valor: ");
              std::tm tm_time = {};
              std::istringstream ss(fechaTexto);
              ss >> std::get_time(&tm_time, "%Y-%m-%d %H:%M:%S");

              if (ss.fail()) {
                cout << "Formato de fecha incorrecto." << endl;
              } else {
                std::time_t timestamp = std::mktime(&tm_time);
                sesion.setHoraInicio(timestamp);
              }
              break;
            }
            default:
              cout << "Opción no válida" << endl << "Vuelva a intentarlo";
          }

          if (db.actualizarSesion(id, sesion)) {
            salida = true;
          }
        }
        break;
      }
      case 4: {
        bool salida = false;
        while (!salida) {
          int id = consola.pedirEntero(
              "Introduzca la id del objeto que desea eliminar: ");
          Sesion sesion = db.obtenerSesion(id);
          if (sesion.getId() == -1) {
            cerr << "La id no es valida" << endl;
            break;
          }

          consola.mostrarSesion(sesion);
          string confirmacion = consola.pedirCadena(
              "Para confirmar su eliminacion, escriba el titulo de la pelicula "
              "de la sesion: ");
          if (confirmacion == sesion.getPelicula().getTitulo()) {
            cout << "Eliminando objeto" << endl;
            if (db.eliminarSesion(id)) {
              cout << "Objeto eliminado" << endl;
            } else {
              cerr << "Error al eliminar el objeto" << endl;
            }
          }
          salida = true;
        }
        break;
      }
      case 5:
        fin = true;
        break;
    }
  }
}

void AdminController::mostrarReporteOcupacion() {
  int opcion = consola.pedirEntero(
      "¿Desea ver las reservas de un cine o de toda la base de datos? "
      "(1-cine, 2-todas):");
  vector<Sesion> sesiones;
  string mensaje = "";
  if (opcion == 1) {
    int idCine =
        consola.pedirEntero("Introduzca el id del cine que desea revisa: ");
    Cine cine = db.obtenerCine(idCine);
    if (cine.getId() == -1) {
      cout << "Id no valido" << endl;
      return;
    }
    mensaje = cine.getNombre();
    sesiones = db.obtenerSesionesDeCine(idCine);
  } else {
    sesiones = db.obtenerSesiones();
  }

  int ocupados = 0, total = 0, capacidad;
  for (auto sesion : sesiones) {
    ocupados += db.obtenerReservasDeSesion(sesion.getId()).size();

    Sala sala = db.obtenerSala(sesion.getIdSala());
    capacidad = sala.getFilas() * sala.getColumnas();
    total += capacidad;
  }
  consola.mostrarReporte(total, ocupados, mensaje);
}