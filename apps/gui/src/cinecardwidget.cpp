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

  QLabel* labelValoracion = new QLabel("★ 4.5  •  Premium", this);
  labelValoracion->setStyleSheet(
      "color: #E5C07B; font-size: 11px; font-weight: bold;");
  layout->addWidget(labelValoracion);

  QPushButton* boton = new QPushButton("Seleccionar Cine", this);
  boton->setStyleSheet(
      "QPushButton { background-color: #E5C07B; color: #12141D; font-weight: bold; padding: "
      "8px; border-radius: 6px; font-size: 12px; border: none; } "
      "QPushButton:hover { background-color: #D4AF37; }");
  layout->addWidget(boton);

  connect(boton, &QPushButton::clicked, this, &CineCardWidget::alPulsarBoton);

  layout->setContentsMargins(10, 10, 10, 10);
  layout->setSpacing(6);
}

CineCardWidget::~CineCardWidget() {}

void CineCardWidget::alPulsarBoton() { emit cineSeleccionado(idCine); }
