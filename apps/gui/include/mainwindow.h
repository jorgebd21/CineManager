#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QListWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QScroller>
#include <QScrollBar>
#include <QVariant>
#include <iomanip>
#include <sstream>
#include <set>

#include "db/datamanager.hpp"
#include "models/usuario.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow(QWidget* parent = nullptr);
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

 private:
  void actualizarBotonUsuario();

  Ui::MainWindow* ui;
  DataManager db;
  Usuario usuarioActual;
  QPushButton* btnUsuario;

  int idCineSeleccionado;
  int idPeliculaSeleccionada;
  int idSesionSeleccionada;
  std::set<std::pair<int, int>> butacasSeleccionadas;
};

#endif  // MAINWINDOW_H