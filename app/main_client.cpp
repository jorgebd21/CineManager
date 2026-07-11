#include "UI/consola.hpp"
#include "controller/clientcontroller.hpp"
#include "db/datamanager.hpp"

using namespace std;

int main() {
  Consola consola;
  DataManager db;
  ClientController controller(db, consola);

  controller.ejecutar();

  return 0;
}