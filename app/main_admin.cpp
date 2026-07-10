#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "UI/consola.hpp"
#include "db/datamanager.hpp"
#include "models/cine.hpp"
#include "models/pelicula.hpp"
#include "models/sala.hpp"
#include "models/sesion.hpp"

using namespace std;

int main() {
  Consola consola;

  DataManager db;

  db.abrirSQL();

  bool volver, fin;
  int id;

  while (true) {
    consola.mostrarMenuAdmin();

    int opcion;
    cin >> opcion;

    switch (opcion) {
      case 1:
        volver = false;
        while (!volver) {
          consola.mostrarSubmenuCines();
          int opcionSubMenu;
          cin >> opcionSubMenu;
          cin.ignore();

          switch (opcionSubMenu) {
            case 1:  // Listar
              for (auto cine : db.obtenerCines()) {
                cout << "ID: " << cine.getId() << endl;
                cout << "Nombre: " << cine.getNombre() << endl;
                cout << "Direccion: " << cine.getDireccion() << endl;
                cout << "==========================================" << endl;
              }
              break;
            case 2:  // Añadir
              fin = false;
              while (!fin) {
                cout << "Introduzca el nombre del cine: " << endl;
                string nombre;
                getline(cin, nombre);
                cout << "Introduzca la direccion del cine: " << endl;
                string direccion;
                getline(cin, direccion);
                Cine cine(-1, nombre, direccion);
                fin = db.crearCine(cine);
                if (fin) {
                  cout << "Se ha creado exitosamente" << endl;
                } else {
                  cout << "Fallo en la creacion, vuelvalo a intentar" << endl;
                }
              }
              break;
            case 3:  // Modificar
              fin = false;
              while (!fin) {
                cout << "Introduzca la id del objeto que quiere modificar: "
                     << endl;
                cin >> id;
                Cine cine = db.obtenerCine(id);
                if (cine.getId() == -1) {
                  cout << "La id no es valida, desea volverlo a intentar (Y/N)"
                       << endl;
                  char respuesta;
                  cin >> respuesta;
                  if (respuesta == 'N') {
                    break;
                  }
                }

                bool fin_modificacion = false;
                while (!fin_modificacion) {
                  cout << "Eliga que valor desea modificar (1-nombre, "
                          "2-direccion)";
                  int seleccion;
                  cin >> seleccion;
                  cout << "Introduzca el nuevo valor: " << endl;
                  string nuevo_valor;
                  cin.ignore();
                  getline(cin, nuevo_valor);

                  switch (seleccion) {
                    case 1:
                      cine.setNombre(nuevo_valor);
                      break;
                    case 2:
                      cine.setDireccion(nuevo_valor);
                      break;
                    default:
                      cout << "Opción no válida" << endl;
                      cout << "Vuelva a intentarlo\n\n";
                  }

                  if (db.actualizarCine(id, cine)) {
                    fin_modificacion = true;
                    fin = true;
                  }
                }
              }
              break;
            case 4:  // Eliminar
              fin = false;
              while (!fin) {
                cout << "Introduzca la id del objeto que desea eliminar: "
                     << endl;
                cin >> id;
                Cine cine = db.obtenerCine(id);
                if (cine.getId() == -1) {
                  cout << "El objeto no existe en la base de datos" << endl;
                  continue;
                }

                cout << "Vas a eliminar el objeto:" << endl;
                cout << "ID: " << cine.getId() << endl;
                cout << "Nombre: " << cine.getNombre() << endl;
                cout << "Direccion: " << cine.getDireccion() << endl;
                cout << "==========================================" << endl;
                cout << "Para confirmar su eliminacion, escriba el nombre del "
                        "cine: "
                     << endl;
                cin.ignore();
                string confirmacion;
                getline(cin, confirmacion);
                if (confirmacion == cine.getNombre()) {
                  cout << "Eliminando objeto" << endl;
                  if (db.eliminarCine(id)) {
                    cout << "Objeto eliminado" << endl;
                  } else {
                    cout << "Error al eliminar el objeto" << endl;
                  }
                }
                fin = true;
              }
              break;
            case 5:  // Volver
              volver = true;
              break;
          }
        }
        break;
      case 2:
        volver = false;
        while (!volver) {
          consola.mostrarSubmenuPeliculas();
          int opcionSubMenu;
          cin >> opcionSubMenu;
          cin.ignore();

          switch (opcionSubMenu) {
            case 1:  // Listar
              for (auto pelicula : db.obtenerPeliculas()) {
                consola.mostrarPelicula(pelicula);
              }
              break;
            case 2:  // Añadir
              fin = false;
              while (!fin) {
                cout << "Introduzca el titulo: " << endl;
                string titulo;
                getline(cin, titulo);
                cout << "Introduzca la duracion: " << endl;
                int duracion;
                cin >> duracion;
                cout << "1-ACCION\n2-COMEDIA\n3-DRAMA\n4-TERROR\n5-CIENCIA_"
                        "FICCION\n6-ROMANCE\n7-DOCUMENTAL\n8-OTHER"
                     << endl;
                cout << "Introduzca el numero del genero: " << endl;
                int genero_numero;
                cin >> genero_numero;
                Genero genero;
                switch (genero_numero) {
                  case 1:
                    genero = Genero::ACCION;
                    break;
                  case 2:
                    genero = Genero::COMEDIA;
                    break;
                  case 3:
                    genero = Genero::DRAMA;
                    break;
                  case 4:
                    genero = Genero::TERROR;
                    break;
                  case 5:
                    genero = Genero::CIENCIA_FICCION;
                    break;
                  case 6:
                    genero = Genero::ROMANCE;
                    break;
                  case 7:
                    genero = Genero::DOCUMENTAL;
                    break;
                  default:
                    genero = Genero::OTHER;
                    break;
                }
                Pelicula pelicula(-1, titulo, genero, duracion);
                if (db.crearPelicula(pelicula)) {
                  cout << "Se ha creado exitosamente" << endl;
                  fin = true;
                } else {
                  cout << "Fallo en la creacion, vuelvalo a intentar" << endl;
                }
              }
              break;
            case 3:  // Modificar
              fin = false;
              while (!fin) {
                cout << "Introduzca la id del objeto que quiere modificar: "
                     << endl;
                cin >> id;
                Pelicula pelicula = db.obtenerPelicula(id);
                if (pelicula.getId() == -1) {
                  cout << "La id no es valida, desea volverlo a intentar (Y/N)"
                       << endl;
                  char respuesta;
                  cin >> respuesta;
                  if (respuesta == 'N') {
                    break;
                  }
                }

                bool fin_modificacion = false;
                while (!fin_modificacion) {
                  cout << "Eliga que valor desea modificar (1-titulo, "
                          "2-duracion, 3-genero)";
                  int seleccion;
                  cin >> seleccion;
                  cout << "Introduzca el nuevo valor: " << endl;

                  switch (seleccion) {
                    case 1: {
                      string titulo;
                      cin.ignore();
                      getline(cin, titulo);
                      pelicula.setTitulo(titulo);
                      break;
                    }
                    case 2:
                      int duracion;
                      cin >> duracion;
                      pelicula.setDuracion(duracion);
                      break;
                    case 3:
                      cout
                          << "1-ACCION\n2-COMEDIA\n3-DRAMA\n4-TERROR\n5-"
                             "CIENCIA_FICCION\n6-ROMANCE\n7-DOCUMENTAL\n8-OTHER"
                          << endl;
                      cout << "Introduzca el numero del genero: " << endl;
                      int genero_numero;
                      cin >> genero_numero;
                      Genero genero;
                      switch (genero_numero) {
                        case 1:
                          genero = Genero::ACCION;
                          break;
                        case 2:
                          genero = Genero::COMEDIA;
                          break;
                        case 3:
                          genero = Genero::DRAMA;
                          break;
                        case 4:
                          genero = Genero::TERROR;
                          break;
                        case 5:
                          genero = Genero::CIENCIA_FICCION;
                          break;
                        case 6:
                          genero = Genero::ROMANCE;
                          break;
                        case 7:
                          genero = Genero::DOCUMENTAL;
                          break;
                        default:
                          genero = Genero::OTHER;
                          break;
                      }
                      pelicula.setGenero(genero);
                      break;
                    default:
                      cout << "Opción no válida" << endl;
                      cout << "Vuelva a intentarlo\n\n";
                  }

                  if (db.actualizarPelicula(id, pelicula)) {
                    fin_modificacion = true;
                    fin = true;
                  }
                }
              }
              break;
            case 4:  // Eliminar
              fin = false;
              while (!fin) {
                cout << "Introduzca la id del objeto que desea eliminar: "
                     << endl;
                cin >> id;
                Pelicula pelicula = db.obtenerPelicula(id);
                if (pelicula.getId() == -1) {
                  cout << "El objeto no existe en la base de datos" << endl;
                  continue;
                }

                cout << "Vas a eliminar el objeto:" << endl;
                consola.mostrarPelicula(pelicula);
                cout << "Para confirmar su eliminacion, escriba el titulo de "
                        "la pelicula: "
                     << endl;
                cin.ignore();
                string confirmacion;
                getline(cin, confirmacion);
                if (confirmacion == pelicula.getTitulo()) {
                  cout << "Eliminando objeto" << endl;
                  if (db.eliminarPelicula(id)) {
                    cout << "Objeto eliminado" << endl;
                  } else {
                    cout << "Error al eliminar el objeto" << endl;
                  }
                }
                fin = true;
              }
              break;
            case 5:  // Volver
              volver = true;
              break;
          }
        }
        break;
      case 3:
        volver = false;
        while (!volver) {
          consola.mostrarSubmenuSesiones();
          int opcionSubMenu;
          cin >> opcionSubMenu;
          cin.ignore();

          switch (opcionSubMenu) {
            case 1:  // Listar
              for (auto sala : db.obtenerSalas()) {
                consola.mostrarSala(sala);
              }
              break;
            case 2:  // Añadir
              fin = false;
              while (!fin) {
                cout << "Introduzca el id del cine: " << endl;
                int cineId;
                cin >> cineId;
                cout << "Introduzca el numero de la sala: " << endl;
                int numeroSala;
                cin >> numeroSala;
                cout << "Introduzca el numero de filas: " << endl;
                int filas;
                cin >> filas;
                cout << "Introduzca el numero de columnas: " << endl;
                int columnas;
                cin >> columnas;
                Sala sala(-1, cineId, numeroSala, filas, columnas);
                fin = db.crearSala(sala);
                if (fin) {
                  cout << "Se ha creado exitosamente" << endl;
                } else {
                  cout << "Fallo en la creacion, vuelvalo a intentar" << endl;
                }
              }
              break;
            case 3:  // Modificar
              fin = false;
              while (!fin) {
                cout << "Introduzca la id del objeto que quiere modificar: "
                     << endl;
                cin >> id;
                Sala sala = db.obtenerSala(id);
                if (sala.getId() == -1) {
                  cout << "La id no es valida, desea volverlo a intentar (Y/N)"
                       << endl;
                  char respuesta;
                  cin >> respuesta;
                  if (respuesta == 'N') {
                    break;
                  }
                }

                bool fin_modificacion = false;
                while (!fin_modificacion) {
                  cout << "Eliga que valor desea modificar (1-NumeroSala, "
                          "2-Filas, 3-Columnas)";
                  int seleccion;
                  cin >> seleccion;
                  cout << "Introduzca el nuevo valor: " << endl;
                  int nuevo_valor;
                  cin >> nuevo_valor;

                  switch (seleccion) {
                    case 1:
                      sala.setNumeroSala(nuevo_valor);
                      break;
                    case 2:
                      sala.setFilas(nuevo_valor);
                      break;
                    case 3:
                      sala.setColumnas(nuevo_valor);
                      break;
                    default:
                      cout << "Opción no válida" << endl;
                      cout << "Vuelva a intentarlo\n\n";
                  }

                  if (db.actualizarSala(id, sala)) {
                    fin_modificacion = true;
                    fin = true;
                  }
                }
              }
              break;
            case 4:  // Eliminar
              fin = false;
              while (!fin) {
                cout << "Introduzca la id del objeto que desea eliminar: "
                     << endl;
                cin >> id;
                Sala sala = db.obtenerSala(id);
                if (sala.getId() == -1) {
                  cout << "El objeto no existe en la base de datos" << endl;
                  continue;
                }

                cout << "Vas a eliminar el objeto:" << endl;
                consola.mostrarSala(sala);
                cout << "Para confirmar su eliminacion, escriba el numero de "
                        "la sala con el siguiente formato (SALA x): "
                     << endl;
                cin.ignore();
                string confirmacion;
                getline(cin, confirmacion);
                if (confirmacion == "SALA " + to_string(sala.getNumeroSala())) {
                  cout << "Eliminando objeto" << endl;
                  if (db.eliminarSala(id)) {
                    cout << "Objeto eliminado" << endl;
                  } else {
                    cout << "Error al eliminar el objeto" << endl;
                  }
                }
                fin = true;
              }
              break;
            case 5:  // Volver
              volver = true;
              break;
          }
        }
        break;
      case 4:
        volver = false;
        while (!volver) {
          consola.mostrarSubmenuSesiones();
          int opcionSubMenu;
          cin >> opcionSubMenu;
          cin.ignore();

          switch (opcionSubMenu) {
            case 1:  // Listar
              for (auto sesion : db.obtenerSesiones()) {
                cout << "ID: " << sesion.getId() << endl;
                cout << "Pelicula: " << sesion.getPelicula().getTitulo()
                     << endl;
                cout << "Sala: " << sesion.getIdSala() << endl;
                std::time_t hora = sesion.getHoraInicio();
                cout << "Hora: "
                     << std::put_time(std::localtime(&hora),
                                      "%Y-%m-%d %H:%M:%S")
                     << endl;
                cout << "==========================================" << endl;
              }
              break;
            case 2:  // Añadir
              fin = false;
              while (!fin) {
                cout << "Introduzca la id de la pelicula: " << endl;
                int id;
                cin >> id;
                Pelicula pelicula = db.obtenerPelicula(id);
                if (pelicula.getId() != -1) {
                  cout << "Introduzca el id de la sala: " << endl;
                  int idSala;
                  cin >> idSala;
                  string fecha_texto;
                  getline(cin, fecha_texto);
                  std::tm tm_time = {};
                  std::istringstream ss(fecha_texto);
                  ss >> std::get_time(&tm_time, "%Y-%m-%d %H:%M:%S");
                  if (ss.fail()) {
                    cout << "Formato de fecha incorrecto." << endl;
                  } else {
                    std::time_t timestamp = std::mktime(&tm_time);
                    Sesion sesion(-1, pelicula, idSala, timestamp);
                    fin = db.crearSesion(sesion);
                    if (fin) {
                      cout << "Se ha creado exitosamente" << endl;
                    } else {
                      cout << "Fallo en la creacion, vuelvalo a intentar"
                           << endl;
                    }
                  }
                } else {
                  cout << "Id invalido" << endl;
                }
              }
              break;
            case 3:  // Modificar
              fin = false;
              while (!fin) {
                cout << "Introduzca la id del objeto que quiere modificar: "
                     << endl;
                cin >> id;
                Sesion sesion = db.obtenerSesion(id);
                if (sesion.getId() == -1) {
                  cout << "La id no es valida, desea volverlo a intentar (Y/N)"
                       << endl;
                  char respuesta;
                  cin >> respuesta;
                  if (respuesta == 'N') {
                    break;
                  }
                }

                bool fin_modificacion = false;
                while (!fin_modificacion) {
                  cout << "Eliga que valor desea modificar (1-Pelicula, "
                          "2-Sala, 3-Fecha/Hora)";
                  int seleccion;
                  cin >> seleccion;
                  cout << "Introduzca el nuevo valor: " << endl;
                  switch (seleccion) {
                    case 1: {
                      int nuevaPeliId;
                      cin >> nuevaPeliId;
                      Pelicula p = db.obtenerPelicula(nuevaPeliId);
                      if (p.getId() != -1) {
                        sesion.setPelicula(p);
                      } else {
                        cout << "Película no encontrada." << endl;
                      }
                      break;
                    }
                    case 2: {
                      int nuevaSalaId;
                      cin >> nuevaSalaId;

                      // Validamos si la sala existe
                      Sala s = db.obtenerSala(nuevaSalaId);
                      if (s.getId() != -1) {
                        sesion.setIdSala(nuevaSalaId);
                      } else {
                        cout << "Sala no encontrada." << endl;
                      }
                      break;
                    }
                    case 3: {
                      cin.ignore();
                      string fecha_texto;
                      getline(cin, fecha_texto);

                      std::tm tm_time = {};
                      std::istringstream ss(fecha_texto);
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
                      cout << "Opción no válida" << endl;
                      cout << "Vuelva a intentarlo\n\n";
                  }

                  if (db.actualizarSesion(id, sesion)) {
                    fin_modificacion = true;
                    fin = true;
                  }
                }
              }
              break;
            case 4:  // Eliminar
              fin = false;
              while (!fin) {
                cout << "Introduzca la id del objeto que desea eliminar: "
                     << endl;
                cin >> id;
                Sesion sesion = db.obtenerSesion(id);
                if (sesion.getId() == -1) {
                  cout << "El objeto no existe en la base de datos" << endl;
                  continue;
                }

                cout << "Vas a eliminar el objeto:" << endl;
                cout << "ID: " << sesion.getId() << endl;
                cout << "Pelicula: " << sesion.getPelicula().getTitulo()
                     << endl;
                cout << "Sala: " << sesion.getIdSala() << endl;
                std::time_t hora = sesion.getHoraInicio();
                cout << "Hora: "
                     << std::put_time(std::localtime(&hora),
                                      "%Y-%m-%d %H:%M:%S")
                     << endl;
                cout << "==========================================" << endl;
                cout << "Para confirmar su eliminacion, escriba el titulo de "
                        "la pelicula de la sesion: "
                     << endl;
                cin.ignore();
                string confirmacion;
                getline(cin, confirmacion);
                if (confirmacion == sesion.getPelicula().getTitulo()) {
                  cout << "Eliminando objeto" << endl;
                  if (db.eliminarSesion(id)) {
                    cout << "Objeto eliminado" << endl;
                  } else {
                    cout << "Error al eliminar el objeto" << endl;
                  }
                }
                fin = true;
              }
              break;
            case 5:  // Volver
              volver = true;
              break;
          }
        }
        break;
      case 5:
        volver = false;
        while (!volver) {
          cout << "Desea ver las reservas de un cine o te toda la base de "
                  "datos (1-cine, 2-todas):"
               << endl;
          int seleccion;
          cin >> seleccion;
          if (seleccion == 1) {
            cout << "Introduzca el id del cine que desea revisa: " << endl;
            int cineId;
            cin >> cineId;
            Cine cine = db.obtenerCine(cineId);
            if (cine.getId() == -1) {
              cout << "Id no valido" << endl;
              break;
            }

            vector<Sesion> sesiones = db.obtenerSesionesDeCine(cineId);
            int ocupados = 0, total = 0, capacidad;
            for (auto sesion : sesiones) {
              ocupados += db.obtenerReservasDeSesion(sesion.getId()).size();

              Sala sala = db.obtenerSala(sesion.getIdSala());
              capacidad = sala.getFilas() * sala.getColumnas();
              total += capacidad;
            }

            cout << endl
                 << "=== Reporte de Ocupación: " << cine.getNombre()
                 << " ===" << endl;
            cout << "Entradas Vendidas: " << ocupados << " / " << total << endl;
            if (total > 0) {
              double porcentaje = (ocupados * 100.0) / total;
              cout << "Porcentaje de Ocupación: " << porcentaje << "%" << endl;
            } else {
              cout << "Este cine no tiene sesiones programadas." << endl;
            }
            cout << "==========================================" << endl;
            volver = true;
          } else {
            vector<Cine> cines = db.obtenerCines();
            int ocupados = 0, total = 0, capacidad;
            for (auto cine : cines) {
              vector<Sesion> sesiones = db.obtenerSesionesDeCine(cine.getId());
              for (auto sesion : sesiones) {
                ocupados += db.obtenerReservasDeSesion(sesion.getId()).size();

                Sala sala = db.obtenerSala(sesion.getIdSala());
                capacidad = sala.getFilas() * sala.getColumnas();
                total += capacidad;
              }
            }

            cout << endl << "=== Reporte de Ocupación ===" << endl;
            cout << "Entradas Vendidas: " << ocupados << " / " << total << endl;
            if (total > 0) {
              double porcentaje = (ocupados * 100.0) / total;
              cout << "Porcentaje de Ocupación: " << porcentaje << "%" << endl;
            } else {
              cout << "Este cine no tiene sesiones programadas." << endl;
            }
            cout << "==========================================" << endl;
            volver = true;
          }
        }
        break;
      case 6:
        cout << "Gracias por usar el sistema administrador de gestión de cine"
             << endl;
        db.cerrarSQL();
        return 0;
      default:
        cout << "Opción no válida" << endl;
        cout << "Vuelva a intentarlo\n\n";
    }
  }

  return 0;
}