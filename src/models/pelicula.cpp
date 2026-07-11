#include "models/pelicula.hpp"

Genero intToGenero(int generoN) {
  switch (generoN) {
    case 1:
      return Genero::NONE;
    case 2:
      return Genero::ACCION;
    case 3:
      return Genero::COMEDIA;
    case 4:
      return Genero::DRAMA;
    case 5:
      return Genero::TERROR;
    case 6:
      return Genero::CIENCIA_FICCION;
    case 7:
      return Genero::ROMANCE;
    case 8:
      return Genero::DOCUMENTAL;
    default:
      return Genero::OTHER;
  }
}

Genero stringToGenero(const std::string& generoStr) {
  if (generoStr == "NONE") return Genero::NONE;
  if (generoStr == "ACCION" || generoStr == "ACCIÓN") return Genero::ACCION;
  if (generoStr == "COMEDIA") return Genero::COMEDIA;
  if (generoStr == "DRAMA") return Genero::DRAMA;
  if (generoStr == "TERROR") return Genero::TERROR;
  if (generoStr == "CIENCIA_FICCION" || generoStr == "CIENCIA-FICCION")
    return Genero::CIENCIA_FICCION;
  if (generoStr == "ROMANCE") return Genero::ROMANCE;
  if (generoStr == "DOCUMENTAL") return Genero::DOCUMENTAL;

  return Genero::OTHER;
}

std::string generoToString(Genero genero) {
  switch (genero) {
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

Pelicula::Pelicula(int id, const std::string& titulo, Genero genero,
                   int duracion)
    : id(id), titulo(titulo), genero(genero), duracion(duracion) {}

int Pelicula::getId() const { return id; }

std::string Pelicula::getTitulo() const { return titulo; }

Genero Pelicula::getGenero() const { return genero; }

int Pelicula::getDuracion() const { return duracion; }

void Pelicula::setTitulo(std::string titulo) { this->titulo = titulo; }

void Pelicula::setGenero(Genero genero) { this->genero = genero; }

void Pelicula::setDuracion(int duracion) { this->duracion = duracion; }