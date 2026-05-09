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

class Pelicula{
private:
    static int contador;

    int id;
    std::string titulo;
    Genero genero;
    int duracion;
    
public:
    Pelicula(const std::string& titulo = "", Genero genero = Genero::NONE, int duracion = 0);
    
    int getId() const;
    std::string getTitulo() const;
    Genero getGenero() const;
    int getDuracion() const;
};

#endif