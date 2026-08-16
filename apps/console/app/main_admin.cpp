#include "UI/consola.hpp"
#include "controller/admincontroller.hpp"
#include "db/datamanager.hpp"

int main() {
  Consola consola;
  DataManager db;
  AdminController controlador(db, consola);

  controlador.ejecutar();

  return 0;
}