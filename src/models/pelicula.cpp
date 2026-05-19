#include "models/pelicula.hpp"

int Pelicula::contador = 0;

Genero stringToGenero(const std::string& generoStr) {
    if (generoStr == "ACCION" || generoStr == "ACCIÓN") return Genero::ACCION;
    if (generoStr == "COMEDIA") return Genero::COMEDIA;
    if (generoStr == "DRAMA") return Genero::DRAMA;
    if (generoStr == "TERROR") return Genero::TERROR;
    if (generoStr == "CIENCIA_FICCION" || generoStr == "CIENCIA-FICCION") return Genero::CIENCIA_FICCION;
    if (generoStr == "ROMANCE") return Genero::ROMANCE;
    if (generoStr == "DOCUMENTAL") return Genero::DOCUMENTAL;
 
    return Genero::OTHER;
}

Pelicula::Pelicula(const std::string& titulo, Genero genero, int duracion) {
    id = ++contador;
    this->titulo = titulo;
    this->genero = genero;
    this->duracion = duracion;
}
        
int Pelicula::getId() const{
    return id;
}

std::string Pelicula::getTitulo() const{
    return titulo;
}

Genero Pelicula::getGenero() const{
    return genero;
}

int Pelicula::getDuracion() const{
    return duracion;
}