#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QScroller>
#include <QScrollBar>
#include <QVariant>
#include <set>
#include <vector>

#include "apiclient.h"
#include "models/cine.hpp"
#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include "models/sesion.hpp"
#include "models/usuario.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 private slots:
  void alSeleccionarCine(int idCine);
  void alSeleccionarPelicula(int idPelicula);
  void alSeleccionarSesionPorId(int idSesion);
  void alPulsarBotonSesion();
  void alPulsarButaca();
  void alPulsarInicio();
  void alFiltrarPeliculas();
  void alConfirmarCompra();
  void alPulsarBotonUsuario();

 protected:
  void resizeEvent(QResizeEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

 private:
  void cargarCines();
  void actualizarBotonUsuario();
  void actualizarFondo();

  Ui::MainWindow* ui;
  ApiClient* api;
  Usuario usuarioActual;
  QPushButton* btnUsuario;
  QPixmap pixmapFondoOriginal;

  int idCineSeleccionado{-1};
  int idPeliculaSeleccionada{-1};
  int idSesionSeleccionada{-1};

  QList<Cine> listaCines;
  QList<Pelicula> listaPeliculas;
  QList<Sesion> listaSesiones;

  std::set<std::pair<int, int>> butacasSeleccionadas;
};

#endif  // MAINWINDOW_H