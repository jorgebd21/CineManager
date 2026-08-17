#ifndef CINECARDWIDGET_H
#define CINECARDWIDGET_H

#include <QWidget>

#include "models/cine.hpp"

class CineCardWidget : public QWidget {
  Q_OBJECT

 public:
  CineCardWidget(const Cine& cine, QWidget* parent = nullptr);
  ~CineCardWidget();

  QSize sizeHint() const override { return QSize(270, 315); }

 signals:
  void cineSeleccionado(int idCine);

 private slots:
  void alPulsarBoton();

 private:
  int idCine;
};

#endif  // CINECARDWIDGET_H
