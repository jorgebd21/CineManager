#include "models/pelicula.hpp"

int Pelicula::contador = 0;

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