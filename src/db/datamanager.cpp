#include "db/datamanager.hpp"
#include <iostream>
#include <filesystem>

using namespace std;

std::string generoToString(Genero genero){
    switch(genero){
        case Genero::NONE:
            return "NONE";
        case Genero::ACCION:
            return "ACCION";
        case Genero::COMEDIA:
            return "COMEDIA";
        case Genero::DRAMA:
            return "DRAMA";
        case Genero::TERROR:
            return "TERROR";
        case Genero::CIENCIA_FICCION:
            return "CIENCIA_FICCION";
        case Genero::ROMANCE:
            return "ROMANCE";
        case Genero::DOCUMENTAL:
            return "DOCUMENTAL";
        default:
            return "OTHER";
    }
}

bool DataManager::abrirSQL() {
    std::string dbPath = "../data/cine.db"; // Fallback por defecto

    // 1. Intentar buscar relativo al ejecutable
    try {
        std::filesystem::path exePath = std::filesystem::read_symlink("/proc/self/exe");
        std::filesystem::path dbTry = exePath.parent_path() / "../.." / "data" / "cine.db";
        if (std::filesystem::exists(dbTry)) {
            dbPath = std::filesystem::canonical(dbTry).string();
        }
    } catch (...) {
        // Ignorar excepciones y continuar con otras opciones
    }
    if (!std::filesystem::exists(dbPath)) {
        std::vector<std::string> potentialPaths = {
            "data/cine.db",
            "../data/cine.db",
            "../../data/cine.db"
        };
        for (const auto& path : potentialPaths) {
            if (std::filesystem::exists(path)) {
                dbPath = path;
                break;
            }
        }
    }

    int rc = sqlite3_open(dbPath.c_str(), &db);

    if (rc != SQLITE_OK) {
        std::cerr << "No se pudo abrir la base de datos: " << sqlite3_errmsg(db) << std::endl;
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
        cout << "No se ha conseguido obtener el cine correctamente" << endl;
        sqlite3_finalize(stmt);
        return Cine(-1, "", "");;
    }else{
        std::string nombre = (const char*)sqlite3_column_text(stmt, 0);
        std::string direccion = (const char*)sqlite3_column_text(stmt, 1);
        sqlite3_finalize(stmt);
        return Cine(id, nombre, direccion);
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
        cout << "No se ha conseguido obtener la sala correctamente" << endl;
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
        cout << "No se ha conseguido obtener la sesion correctamente" << endl;
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

std::vector<Sala> DataManager::obtenerSalasDeCine(int idCine) {
    const char* query = "SELECT id, cine_id, numero_sala, filas, columnas FROM salas WHERE cine_id = ?";
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
    
    std::vector<Sala> salas;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int cine_id = sqlite3_column_int(stmt, 1);
        int numero_sala = sqlite3_column_int(stmt, 2);
        int fila = sqlite3_column_int(stmt, 3);
        int columna = sqlite3_column_int(stmt, 4);
        salas.push_back(Sala(id, cine_id, numero_sala, fila, columna));
    }
    
    sqlite3_finalize(stmt);
    return salas;
}

std::vector<Pelicula> DataManager::obtenerPeliculas(){
    const char* query = "SELECT id, titulo, genero, duracion FROM peliculas";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return {};
    }
    
    std::vector<Pelicula> peliculas;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string titulo = (const char*)sqlite3_column_text(stmt, 1);
        std::string genero = (const char*)sqlite3_column_text(stmt, 2);
        Genero generoEnum = stringToGenero(genero);
        int duracion = sqlite3_column_int(stmt, 3);
        peliculas.push_back(Pelicula(id, titulo, generoEnum, duracion));
    }
    
    sqlite3_finalize(stmt);
    return peliculas;
}

std::vector<Sesion> DataManager::obtenerSesionesDeCine(int idCine){
    const char* query = "SELECT s.id, s.pelicula_id, s.sala_id, strftime('%s', s.fecha_hora) FROM sesiones s JOIN salas sa ON s.sala_id = sa.id WHERE sa.cine_id = ?";
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

std::vector<Cine> DataManager::obtenerCines(){
    const char* query = "SELECT id, nombre, direccion FROM cines";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return {};
    }
    
    std::vector<Cine> cines;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string nombre = (const char*)sqlite3_column_text(stmt, 1);
        std::string direccion = (const char*)sqlite3_column_text(stmt, 2);
        cines.push_back(Cine(id, nombre, direccion));
    }
    
    sqlite3_finalize(stmt);
    return cines;
}

std::vector<Sala> DataManager::obtenerSalas(){
    const char* query = "SELECT id, cine_id, numero_sala, filas, columnas FROM salas";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return {};
    }
    
    std::vector<Sala> salas;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int cine_id = sqlite3_column_int(stmt, 1);
        int numero_sala = sqlite3_column_int(stmt, 2);
        int fila = sqlite3_column_int(stmt, 3);
        int columna = sqlite3_column_int(stmt, 4);
        salas.push_back(Sala(id, cine_id, numero_sala, fila, columna));
    }
    
    sqlite3_finalize(stmt);
    return salas;
}

std::vector<Sesion> DataManager::obtenerSesiones(){
    const char* query = "SELECT id, pelicula_id, sala_id, strftime('%s', fecha_hora) FROM sesiones";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
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

bool DataManager::crearCine(const Cine& cine) {
    const char* query = "INSERT INTO cines (nombre, direccion) VALUES (?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, cine.getNombre().c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_text(stmt, 2, cine.getDireccion().c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido crear el cine correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::crearSala(const Sala& sala) {
    const char* query = "INSERT INTO salas (cine_id, numero_sala, filas, columnas) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, sala.getCineId());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 2, sala.getNumeroSala());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 3, sala.getFilas());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 4, sala.getColumnas());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido crear la sala correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::crearPelicula(const Pelicula& pelicula) {
    const char* query = "INSERT INTO peliculas (titulo, genero, duracion) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, pelicula.getTitulo().c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_text(stmt, 2, generoToString(pelicula.getGenero()).c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 3, pelicula.getDuracion());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido crear la pelicula correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}
bool DataManager::crearSesion(const Sesion& sesion) {
    const char* query = "INSERT INTO sesiones (pelicula_id, sala_id, fecha_hora, precio_entrada) VALUES (?, ?, datetime(?, 'unixepoch'), 10.0)";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, sesion.getPelicula().getId());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 2, sesion.getIdSala());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 3, sesion.getHoraInicio());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido crear la sesion correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::actualizarCine(int id, const Cine& cine) {
    const char* query = "UPDATE cines SET nombre = ?, direccion = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, cine.getNombre().c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_text(stmt, 2, cine.getDireccion().c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 3, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido actualizar el cine correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::actualizarSala(int id, const Sala& sala) {
    const char* query = "UPDATE salas SET cine_id = ?, numero_sala = ?, filas = ?, columnas = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, sala.getCineId());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 2, sala.getNumeroSala());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 3, sala.getFilas());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 4, sala.getColumnas());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 5, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido actualizar la sala correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::actualizarPelicula(int id, const Pelicula& pelicula) {
    const char* query = "UPDATE peliculas SET titulo = ?, genero = ?, duracion = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, pelicula.getTitulo().c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_text(stmt, 2, generoToString(pelicula.getGenero()).c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 3, pelicula.getDuracion());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 4, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido actualizar la pelicula correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}
bool DataManager::actualizarSesion(int id, const Sesion& sesion) {
    const char* query = "UPDATE sesiones SET pelicula_id = ?, sala_id = ?, fecha_hora = datetime(?, 'unixepoch') WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, sesion.getPelicula().getId());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 2, sesion.getIdSala());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 3, sesion.getHoraInicio());
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    rc = sqlite3_bind_int(stmt, 4, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido actuaizar la sesion correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::actualizarReserva(int id, const Reserva& reserva) {
    const char* query = "UPDATE reservas SET sesion_id = ?, fila = ?, columna = ? WHERE id = ?";
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
    rc = sqlite3_bind_int(stmt, 4, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido actualizar la reserva correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::eliminarCine(int id) {
    const char* query = "DELETE FROM cines WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido eliminar el cine correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::eliminarSala(int id) {
    const char* query = "DELETE FROM salas WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido eliminar la sala correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::eliminarPelicula(int id) {
    const char* query = "DELETE FROM peliculas WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido eliminar la pelicula correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::eliminarSesion(int id) {
    const char* query = "DELETE FROM sesiones WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido eliminar la sesion correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

bool DataManager::eliminarReserva(int id) {
    const char* query = "DELETE FROM reservas WHERE id = ?";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al enlazar parámetro: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE){
        cout << "No se ha conseguido eliminar la reserva correctamente" << endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}