#include "models/pelicula.hpp"

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

Pelicula::Pelicula(int id, const std::string& titulo, Genero genero, int duracion) : id(id), titulo(titulo), genero(genero), duracion(duracion) {}
        
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

void Pelicula::setTitulo(std::string titulo){
    this->titulo = titulo;
}

void Pelicula::setGenero(Genero genero){
    this->genero = genero;
}

void Pelicula::setDuracion(int duracion){
    this->duracion = duracion;
}