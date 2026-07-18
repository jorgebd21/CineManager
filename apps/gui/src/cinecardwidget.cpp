#include "../include/cinecardwidget.h"

#include <QDir>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

CineCardWidget::CineCardWidget(const Cine& cine, QWidget* parent)
    : QWidget(parent), idCine(cine.getId()) {
  QVBoxLayout* layout = new QVBoxLayout(this);

  QLabel* labelImagen = new QLabel(this);
  QString rutaImagen = QString::fromStdString(
      "data/images/cines/" + std::to_string(cine.getId()) + ".jpg");

  if (!QFile::exists(rutaImagen)) {
    rutaImagen = "data/images/peliculas/default.jpg";
  }

  QPixmap pixmap(rutaImagen);
  labelImagen->setPixmap(pixmap.scaled(250, 150, Qt::KeepAspectRatioByExpanding,
                                       Qt::SmoothTransformation));
  labelImagen->setFixedSize(250, 150);
  layout->addWidget(labelImagen);

  QLabel* labelNombre =
      new QLabel(QString::fromStdString(cine.getNombre()), this);
  labelNombre->setStyleSheet(
      "font-weight: bold; font-size: 14px; color: #ffffff;");
  layout->addWidget(labelNombre);

  QLabel* labelDireccion =
      new QLabel(QString::fromStdString(cine.getDireccion()), this);
  labelDireccion->setStyleSheet("color: #abb2bf; font-size: 11px;");
  layout->addWidget(labelDireccion);

  QLabel* labelValoracion = new QLabel("⭐ 4.5  •  Premium", this);
  labelValoracion->setStyleSheet(
      "color: #00f0b5; font-size: 11px; font-weight: bold;");
  layout->addWidget(labelValoracion);

  QPushButton* boton = new QPushButton("SELECT", this);
  boton->setStyleSheet(
      "background-color: #00f0b5; color: #121418; font-weight: bold; padding: "
      "6px; border-radius: 6px;");
  layout->addWidget(boton);

  connect(boton, &QPushButton::clicked, this, &CineCardWidget::alPulsarBoton);

  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(6);
}

CineCardWidget::~CineCardWidget() {}

void CineCardWidget::alPulsarBoton() { emit cineSeleccionado(idCine); }
