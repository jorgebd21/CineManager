#include "db/datamanager.hpp"
#include <iostream>

using namespace std;

bool DataManager::abrirSQL() {
    int rc = sqlite3_open("../data/cine.db", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "No se pudo abrir: " << sqlite3_errmsg(db) << std::endl;
    }
    return rc == SQLITE_OK;
}

void DataManager::cerrarSQL() {
    if(db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}

Pelicula DataManager::obtenerPelicula(int id) {
   const char* query = "SELECT titulo, genero, duracion FROM peliculas WHERE id = ?";
   sqlite3_stmt* stmt;

   int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
   if (rc != SQLITE_OK) {
       std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
       return Pelicula(-1, "", Genero::NONE, 0);
   }

   rc = sqlite3_bind_int(stmt, 1, id);
   if (rc != SQLITE_OK) {
       std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
       sqlite3_finalize(stmt);
       return Pelicula(-1, "", Genero::NONE, 0);
   }

   if (sqlite3_step(stmt) == SQLITE_ROW) {
       std::string titulo = (const char*)sqlite3_column_text(stmt, 0);
       std::string genero = (const char*)sqlite3_column_text(stmt, 1);
       Genero generoEnum = stringToGenero(genero);
       int duracion = sqlite3_column_int(stmt, 2);
       sqlite3_finalize(stmt);
       return Pelicula(id, titulo, generoEnum, duracion);
   }

   sqlite3_finalize(stmt);
   return Pelicula(-1, "", Genero::NONE, 0);
}

std::vector<Pelicula> DataManager::obtenerCartelera(int idCine) {
    const char* query = "SELECT DISTINCT p.id, p.titulo, p.genero, p.duracion FROM peliculas p JOIN sesiones s ON p.id = s.pelicula_id JOIN salas sa ON s.sala_id = sa.id WHERE sa.cine_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return {};
    }
    
    rc = sqlite3_bind_int(stmt, 1, idCine);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return {};
    }
    
    std::vector<Pelicula> cartelera;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string titulo = (const char*)sqlite3_column_text(stmt, 1);
        std::string genero = (const char*)sqlite3_column_text(stmt, 2);
        Genero generoEnum = stringToGenero(genero);
        int duracion = sqlite3_column_int(stmt, 3);
        cartelera.push_back(Pelicula(id, titulo, generoEnum, duracion));
    }
    
    sqlite3_finalize(stmt);
    return cartelera;
}

std::vector<Sesion> DataManager::obtenerSesionesDePelicula(int idCine, int idPelicula) {
    const char* query = "SELECT s.id, s.pelicula_id, s.sala_id, strftime('%s', s.fecha_hora) FROM sesiones s JOIN salas sa ON s.sala_id = sa.id WHERE sa.cine_id = ? AND s.pelicula_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return {};
    }
    
    rc = sqlite3_bind_int(stmt, 1, idCine);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return {};
    }
    
    rc = sqlite3_bind_int(stmt, 2, idPelicula);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return {};
    }
    
    std::vector<Sesion> sesiones;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int pelicula_id = sqlite3_column_int(stmt, 1);
        int sala_id = sqlite3_column_int(stmt, 2);
        int fecha_hora = sqlite3_column_int(stmt, 3);
        const Pelicula pelicula = obtenerPelicula(pelicula_id);
        sesiones.push_back(Sesion(id, pelicula, sala_id, fecha_hora));
    }
    
    sqlite3_finalize(stmt);
    return sesiones;
}

std::vector<Reserva> DataManager::obtenerReservasDeSesion(int idSesion) {
    const char* query = "SELECT id, sesion_id, fila, columna FROM reservas WHERE sesion_id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return {};
    }
    
    rc = sqlite3_bind_int(stmt, 1, idSesion);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return {};
    }
    
    std::vector<Reserva> reservas;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int sesion_id = sqlite3_column_int(stmt, 1);
        int fila = sqlite3_column_int(stmt, 2);
        int columna = sqlite3_column_int(stmt, 3);
        reservas.push_back(Reserva(id, sesion_id, fila, columna));
    }
    
    sqlite3_finalize(stmt);
    return reservas;
}

bool DataManager::crearReserva(const Reserva& reserva){
    const char* query = "INSERT INTO reservas (sesion_id, fila, columna, estado) VALUES (?, ?, ?, 'COMPRADO')";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, reserva.getIdSesion());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 2, reserva.getFila());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 3, reserva.getColumna());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido crear la reserva correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

Cine DataManager::obtenerCine(int id){
    const char* query = "SELECT nombre, direccion FROM cines WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return Cine(-1, "", "");
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return Cine(-1, "", "");
    }

    if(sqlite3_step(stmt) != SQLITE_ROW){
        cout << "No se ha conseguido crear la reserva correctamente" << endl;
        sqlite3_finalize(stmt);
        return Cine(-1, "", "");;
    }else{
        std::string nombre = (const char*)sqlite3_column_text(stmt, 0);
        std::string dirrecion = (const char*)sqlite3_column_text(stmt, 1);
        sqlite3_finalize(stmt);
        return Cine(id, nombre, dirrecion);
    }
}

Sala DataManager::obtenerSala(int id) {
    const char* query = "SELECT cine_id, numero_sala, filas, columnas FROM salas WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return Sala(-1, -1, -1, -1, -1);
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return Sala(-1, -1, -1, -1, -1);
    }

    if(sqlite3_step(stmt) != SQLITE_ROW){
        cout << "No se ha conseguido crear la reserva correctamente" << endl;
        sqlite3_finalize(stmt);
        return Sala(-1, -1, -1, -1, -1);
    }else{
        int cine_id = sqlite3_column_int(stmt, 0);
        int numero_sala = sqlite3_column_int(stmt, 1);
        int filas = sqlite3_column_int(stmt, 2);
        int columnas = sqlite3_column_int(stmt, 3);
        sqlite3_finalize(stmt);
        return Sala(id, cine_id, numero_sala, filas, columnas);
    }
}

Sesion DataManager::obtenerSesion(int id) {
    const char* query = "SELECT pelicula_id, sala_id, strftime('%s', fecha_hora) FROM sesiones WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return Sesion(-1, Pelicula(-1, "", Genero::NONE, 0), -1, 0);
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return Sesion(-1, Pelicula(-1, "", Genero::NONE, 0), -1, 0);
    }

    if(sqlite3_step(stmt) != SQLITE_ROW){
        cout << "No se ha conseguido crear la reserva correctamente" << endl;
        sqlite3_finalize(stmt);
        return Sesion(-1, Pelicula(-1, "", Genero::NONE, 0), -1, 0);
    }else{
        int pelicula_id = sqlite3_column_int(stmt, 0);
        int sala_id = sqlite3_column_int(stmt, 1);
        int fecha_hora = sqlite3_column_int(stmt, 2);
        sqlite3_finalize(stmt);
        return Sesion(id, obtenerPelicula(pelicula_id), sala_id, fecha_hora);
    }
}

bool DataManager::crearCine(const Cine& cine) { return true; }
bool DataManager::crearSala(const Sala& sala) { return true; }
bool DataManager::crearPelicula(const Pelicula& pelicula) { return true; }
bool DataManager::crearSesion(const Sesion& sesion) { return true; }

static Pelicula mockPelicula(1, "Pelicula Mock", Genero::NONE, 120);

Reserva DataManager::obtenerReserva(int id) { return Reserva(id, 1, 1, 1); }

std::vector<Sala> DataManager::obtenerSalasDeCine(int idCine) { return {}; }

bool DataManager::actualizarCine(int id, const Cine& cine) { return true; }
bool DataManager::actualizarSala(int id, const Sala& sala) { return true; }
bool DataManager::actualizarPelicula(int id, const Pelicula& pelicula) { return true; }
bool DataManager::actualizarSesion(int id, const Sesion& sesion) { return true; }
bool DataManager::actualizarReserva(int id, const Reserva& reserva) { return true; }

bool DataManager::eliminarCine(int id) { return true; }
bool DataManager::eliminarSala(int id) { return true; }
bool DataManager::eliminarPelicula(int id) { return true; }
bool DataManager::eliminarSesion(int id) { return true; }
bool DataManager::eliminarReserva(int id) { return true; }