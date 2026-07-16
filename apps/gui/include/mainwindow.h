#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVariant>
#include "db/datamanager.hpp"

#include <iomanip>
#include <sstream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void alSeleccionarCine(QListWidgetItem *item);
    void alSeleccionarPelicula(QListWidgetItem *item);
    void alSeleccionarSesion(QListWidgetItem *item);
    void alPulsarButaca();
    void alPulsarInicio();

private:
    Ui::MainWindow *ui;
    DataManager db;

    int idCineSeleccionado;
    int idPeliculaSeleccionada;
    int idSesionSeleccionada;
};

#endif // MAINWINDOW_H