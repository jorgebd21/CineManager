#ifndef CINE_H
#define CINE_H

#include <string>

class Cine {
 private:
  int id;
  std::string nombre;
  std::string direccion;

 public:
  Cine(int id, const std::string& nombre, const std::string& direccion);

  int getId() const;
  std::string getNombre() const;
  std::string getDireccion() const;

  void setNombre(const std::string& nombre);
  void setDireccion(const std::string& direccion);
};

#endif