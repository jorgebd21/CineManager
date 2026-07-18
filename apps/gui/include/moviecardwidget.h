#ifndef MOVIECARDWIDGET_H
#define MOVIECARDWIDGET_H

#include <QWidget>

#include "models/pelicula.hpp"

class MovieCardWidget : public QWidget {
  Q_OBJECT

 public:
  MovieCardWidget(const Pelicula& pelicula, QWidget* parent = nullptr);
  ~MovieCardWidget();

  QString getTitulo() const { return titulo; }
  QString getGenero() const { return genero; }

 signals:
  void PeliculaSeleccionada(int idPelicula);

 private slots:
  void alPulsarBoton();

 private:
  int idPelicula;
  QString titulo;
  QString genero;
};

#endif  // MOVIECARDWIDGET_H
