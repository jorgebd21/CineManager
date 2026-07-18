#include "../include/moviecardwidget.h"

#include <QFile>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

MovieCardWidget::MovieCardWidget(const Pelicula& pelicula, QWidget* parent)
    : QWidget(parent), idPelicula(pelicula.getId()) {
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

  QLabel* labelTitulo =
      new QLabel(QString::fromStdString(pelicula.getTitulo()), this);
  labelTitulo->setStyleSheet(
      "font-weight: bold; font-size: 13px; color: #ffffff;");
  layout->addWidget(labelTitulo);

    QString infoStr =
      QString::fromStdString(generoToString(pelicula.getGenero())) +
      " • " + QString::number(pelicula.getDuracion()) + " min";
  QLabel* labelInfo = new QLabel(infoStr, this);
  labelInfo->setStyleSheet("color: #abb2bf; font-size: 10px;");
  layout->addWidget(labelInfo);

  QPushButton* boton = new QPushButton("BOOK TICKETS", this);
  boton->setStyleSheet(
      "background-color: #00f0b5; color: #121418; font-weight: bold; padding: "
      "6px; border-radius: 6px; font-size: 11px;");
  layout->addWidget(boton);

  connect(boton, &QPushButton::clicked, this, &MovieCardWidget::alPulsarBoton);

  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(5);
}

MovieCardWidget::~MovieCardWidget() {}

void MovieCardWidget::alPulsarBoton() { emit PeliculaSeleccionada(idPelicula); }
