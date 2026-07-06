#ifndef RESERVA_HPP
#define RESERVA_HPP

enum Estado {OCUPADO, LIBRE};

class Reserva {
private:
    int id;
    int idSesion;
    int fila;
    int columna;
    Estado estado;

public:
    Reserva(){};
    Reserva(int id, int idSesion, int fila, int columna, Estado estado){   
        this->id = id;
        this->idSesion = idSesion;
        this->fila = fila;
        this->columna = columna;
        this->estado = estado;
    };
    
    int getId() const { return id; }
    int getIdSesion() const { return idSesion; }
    int getFila() const { return fila; }
    int getColumna() const { return columna; }
    Estado getEstado() const { return estado; }
    
    void setEstado(Estado estado) { this->estado = estado; }
};

#endif