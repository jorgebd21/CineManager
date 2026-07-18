#ifndef MOVIECARDWIDGET_H
#define MOVIECARDWIDGET_H

#include <QWidget>

#include "models/pelicula.hpp"

class MovieCardWidget : public QWidget {
  Q_OBJECT

 public:
  MovieCardWidget(const Pelicula& pelicula, QWidget* parent = nullptr);
  ~MovieCardWidget();

 signals:
  void PeliculaSeleccionada(int idCine);

 private slots:
  void alPulsarBoton();

 private:
  int idPelicula;
};

#endif  // MOVIECARDWIDGET_H
