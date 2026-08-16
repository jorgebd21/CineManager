#ifndef API_CONTROLLER_HPP
#define API_CONTROLLER_HPP

#include <crow.h>

#include "db/datamanager.hpp"

class ApiController {
 private:
  DataManager& db;

 public:
  explicit ApiController(DataManager& dataManager);

  void registrarRutas(crow::SimpleApp& app);
};

#endif  // API_CONTROLLER_HPP
