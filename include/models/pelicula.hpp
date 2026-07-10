#ifndef PELICULA_H
#define PELICULA_H

#include <string>

enum class Genero {
    NONE,
    ACCION,
    COMEDIA,
    DRAMA,
    TERROR,
    CIENCIA_FICCION,
    ROMANCE,
    DOCUMENTAL,
    OTHER
};

Genero stringToGenero(const std::string& generoStr);
class Pelicula{
private:
    int id;
    std::string titulo;
    Genero genero;
    int duracion;
    
public:
    Pelicula(int id, const std::string& titulo, Genero genero, int duracion);
    
    int getId() const;
    std::string getTitulo() const;
    Genero getGenero() const;
    int getDuracion() const;

    void setTitulo(std::string titulo);
    void setGenero(Genero genero);
    void setDuracion(int duracion);
};

#endif