#include "../include/moviecardwidget.h"

#include <QFile>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

MovieCardWidget::MovieCardWidget(const Pelicula& pelicula, QWidget* parent)
    : QWidget(parent), 
      idPelicula(pelicula.getId()), 
      titulo(QString::fromStdString(pelicula.getTitulo())), 
      genero(QString::fromStdString(generoToString(pelicula.getGenero()))) {
  QVBoxLayout* layout = new QVBoxLayout(this);

  QLabel* labelImagen = new QLabel(this);
  QString rutaImagen = QString::fromStdString(
      "data/images/peliculas/" + std::to_string(pelicula.getId()) + ".jpg");

  if (!QFile::exists(rutaImagen)) {
    rutaImagen = "data/images/peliculas/default.jpg";
  }

  QPixmap pixmap(rutaImagen);

  labelImagen->setPixmap(pixmap.scaled(180, 240, Qt::KeepAspectRatioByExpanding,
                                       Qt::SmoothTransformation));
  labelImagen->setFixedSize(180, 240);
  layout->addWidget(labelImagen);

  QLabel* labelTitulo = new QLabel(QString::fromStdString(pelicula.getTitulo()), this);
  labelTitulo->setStyleSheet(
      "font-weight: bold; font-size: 13px; color: #ffffff; margin-top: 8px; margin-bottom: 2px;");
  layout->addWidget(labelTitulo);

  QString infoStr =
      QString::fromStdString(generoToString(pelicula.getGenero())) +
      " • " + QString::number(pelicula.getDuracion()) + " min";
  QLabel* labelInfo = new QLabel(infoStr, this);
  labelInfo->setStyleSheet("color: #abb2bf; font-size: 10px; margin-bottom: 8px;");
  layout->addWidget(labelInfo);

  QPushButton* boton = new QPushButton("Comprar Entradas", this);
  boton->setStyleSheet(
      "QPushButton { background-color: #E5C07B; color: #12141D; font-weight: bold; padding: "
      "8px; border-radius: 6px; font-size: 11px; border: none; } "
      "QPushButton:hover { background-color: #D4AF37; }");
  layout->addWidget(boton);

  connect(boton, &QPushButton::clicked, this, &MovieCardWidget::alPulsarBoton);

  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(4);
}

MovieCardWidget::~MovieCardWidget() {}

void MovieCardWidget::alPulsarBoton() { emit PeliculaSeleccionada(idPelicula); }
