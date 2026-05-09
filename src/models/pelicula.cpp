#include "models/pelicula.hpp"

Pelicula::Pelicula(int id, const std::string& titulo, Genero genero, int duracion) {
    this->id = id;
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