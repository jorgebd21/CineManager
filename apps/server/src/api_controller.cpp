#include "api_controller.hpp"

#include <ctime>
#include <sstream>

ApiController::ApiController(DataManager& dataManager) : db(dataManager) {}

void ApiController::registrarRutas(crow::SimpleApp& app) {
  // 1. Health check endpoint
  CROW_ROUTE(app, "/api/v1/health")
  ([]() {
    crow::json::wvalue res;
    res["status"] = "ok";
    res["app"] = "CineManager REST API v2.0";
    return crow::response(200, res);
  });

  // 2. Obtener lista de cines
  CROW_ROUTE(app, "/api/v1/cines")
  ([this]() {
    std::vector<Cine> cines = db.obtenerCines();
    std::vector<crow::json::wvalue> listaJson;
    for (const auto& c : cines) {
      crow::json::wvalue item;
      item["id"] = c.getId();
      item["nombre"] = c.getNombre();
      item["direccion"] = c.getDireccion();
      listaJson.push_back(item);
    }
    crow::json::wvalue res;
    res["cines"] = std::move(listaJson);
    return crow::response(200, res);
  });

  // 3. Obtener lista de películas en cartelera
  CROW_ROUTE(app, "/api/v1/peliculas")
  ([this]() {
    std::vector<Pelicula> peliculas = db.obtenerPeliculas();
    std::vector<crow::json::wvalue> listaJson;
    for (const auto& p : peliculas) {
      crow::json::wvalue item;
      item["id"] = p.getId();
      item["titulo"] = p.getTitulo();
      item["genero"] = generoToString(p.getGenero());
      item["duracion"] = p.getDuracion();
      listaJson.push_back(item);
    }
    crow::json::wvalue res;
    res["peliculas"] = std::move(listaJson);
    return crow::response(200, res);
  });

  // 4. Obtener sesiones
  CROW_ROUTE(app, "/api/v1/sesiones")
  ([this](const crow::request& req) {
    char* cineParam = req.url_params.get("cine_id");
    int idCine = cineParam ? std::atoi(cineParam) : 1;

    std::vector<Sesion> sesiones = db.obtenerSesionesDeCine(idCine);
    std::vector<crow::json::wvalue> listaJson;
    for (const auto& s : sesiones) {
      crow::json::wvalue item;
      item["id"] = s.getId();
      item["pelicula_id"] = s.getPelicula().getId();
      item["sala_id"] = s.getIdSala();
      item["fecha_hora"] = s.getHoraInicio();
      listaJson.push_back(item);
    }
    crow::json::wvalue res;
    res["sesiones"] = std::move(listaJson);
    return crow::response(200, res);
  });

  // 5. Iniciar sesión / Login por DNI
  CROW_ROUTE(app, "/api/v1/auth/login").methods(crow::HTTPMethod::POST)
  ([this](const crow::request& req) {
    auto reqJson = crow::json::load(req.body);
    if (!reqJson || !reqJson.has("dni") || !reqJson.has("password")) {
      crow::json::wvalue err;
      err["error"] = "Se requiere DNI y contraseña en formato JSON.";
      return crow::response(400, err);
    }

    std::string dni = std::string(reqJson["dni"].s());
    std::string pass = std::string(reqJson["password"].s());

    Usuario user = db.autenticarUsuario(dni, pass);
    if (user.esValido()) {
      crow::json::wvalue res;
      res["dni"] = user.getDni();
      res["nombre"] = user.getNombre();
      res["apellidos"] = user.getApellidos();
      res["email"] = user.getEmail();
      res["rol"] = user.getRol();
      return crow::response(200, res);
    } else {
      crow::json::wvalue err;
      err["error"] = "DNI o contraseña incorrectos.";
      return crow::response(401, err);
    }
  });

  // 6. Registro de usuario nuevo
  CROW_ROUTE(app, "/api/v1/auth/register").methods(crow::HTTPMethod::POST)
  ([this](const crow::request& req) {
    auto reqJson = crow::json::load(req.body);
    if (!reqJson || !reqJson.has("dni") || !reqJson.has("nombre") ||
        !reqJson.has("apellidos") || !reqJson.has("email") ||
        !reqJson.has("password")) {
      crow::json::wvalue err;
      err["error"] = "Faltan campos obligatorios para el registro.";
      return crow::response(400, err);
    }

    std::string dni = std::string(reqJson["dni"].s());
    if (db.obtenerUsuario(dni).esValido()) {
      crow::json::wvalue err;
      err["error"] = "El DNI ya se encuentra registrado.";
      return crow::response(409, err);
    }

    Usuario nuevoUser(dni, std::string(reqJson["nombre"].s()),
                       std::string(reqJson["apellidos"].s()),
                       std::string(reqJson["email"].s()),
                       std::string(reqJson["password"].s()), "CLIENTE");

    if (db.crearUsuario(nuevoUser)) {
      crow::json::wvalue res;
      res["dni"] = nuevoUser.getDni();
      res["nombre"] = nuevoUser.getNombre();
      res["email"] = nuevoUser.getEmail();
      return crow::response(201, res);
    } else {
      crow::json::wvalue err;
      err["error"] = "Error interno al registrar el usuario en la BD.";
      return crow::response(500, err);
    }
  });

  // 7. Comprar entradas / Crear reservas
  CROW_ROUTE(app, "/api/v1/reservas").methods(crow::HTTPMethod::POST)
  ([this](const crow::request& req) {
    auto reqJson = crow::json::load(req.body);
    if (!reqJson || !reqJson.has("sesion_id") || !reqJson.has("reservas")) {
      crow::json::wvalue err;
      err["error"] = "Se requiere sesion_id y lista de reservas.";
      return crow::response(400, err);
    }

    int idSesion = reqJson["sesion_id"].i();
    auto listaReservas = reqJson["reservas"];

    bool exitoTotal = true;
    int creadas = 0;

    for (const auto& item : listaReservas) {
      int fila = item["fila"].i();
      int columna = item["columna"].i();
      std::string tipo = item.has("tipo") ? std::string(item["tipo"].s()) : "Adulto";
      float precio = item.has("precio") ? (float)item["precio"].d() : 7.50f;

      Reserva r(-1, idSesion, fila, columna, "COMPRADO", std::time(nullptr),
                tipo, precio);
      if (db.crearReserva(r) != -1) {
        creadas++;
      } else {
        exitoTotal = false;
      }
    }

    if (exitoTotal && creadas > 0) {
      crow::json::wvalue res;
      res["status"] = "exito";
      res["reservas_creadas"] = creadas;
      return crow::response(201, res);
    } else {
      crow::json::wvalue err;
      err["error"] = "No se pudieron realizar las reservas (butacas ocupadas o sesión inválida).";
      return crow::response(409, err);
    }
  });
}
