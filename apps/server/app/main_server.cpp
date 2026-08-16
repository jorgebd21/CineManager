#include <crow.h>
#include <iostream>

#include "api_controller.hpp"
#include "db/datamanager.hpp"

int main() {
  std::cout << "=====================================================" << std::endl;
  std::cout << " 🎬 Iniciando Servidor HTTP REST API CineManager v2.0" << std::endl;
  std::cout << "=====================================================" << std::endl;

  DataManager db;
  crow::SimpleApp app;

  ApiController api(db);
  api.registrarRutas(app);

  std::cout << "🚀 Servidor web escuchando en http://localhost:8080" << std::endl;
  std::cout << "📌 Presiona Ctrl+C para detener el servidor." << std::endl;

  app.port(8080).multithreaded().run();

  return 0;
}
