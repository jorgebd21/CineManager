#include "../include/mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
  ui->setupUi(this);
  
  idCineSeleccionado = -1;
  idPeliculaSeleccionada = -1;

  std::vector<Cine> cines = db.obtenerCines();

  for(const auto& cine: cines){
    QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(cine.getNombre()));
    item->setData(Qt::UserRole, cine.getId());
    ui->listaCines->addItem(item);
  }

  connect(ui->listaCines, &QListWidget::itemDoubleClicked, this, &MainWindow::alSeleccionarCine);
  connect(ui->listaPeliculas, &QListWidget::itemDoubleClicked, this, &MainWindow::alSeleccionarPelicula);
  connect(ui->listaSesiones, &QListWidget::itemDoubleClicked, this, &MainWindow::alSeleccionarSesion);
  connect(ui->botonInicio, &QPushButton::clicked, this, &MainWindow::alPulsarInicio);
}

MainWindow::~MainWindow(){
  delete ui;
}

void MainWindow::alSeleccionarCine(QListWidgetItem *item){
  idCineSeleccionado = item->data(Qt::UserRole).toInt();

  std::vector<Pelicula> peliculas = db.obtenerCartelera(idCineSeleccionado);

  ui->listaPeliculas->clear();
  for(const auto& pelicula: peliculas){
    QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(pelicula.getTitulo()));
    item->setData(Qt::UserRole, pelicula.getId());
    ui->listaPeliculas->addItem(item);
  }

  ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::alSeleccionarPelicula(QListWidgetItem *item){
  idPeliculaSeleccionada = item->data(Qt::UserRole).toInt();

  std::vector<Sesion> sesiones = db.obtenerSesionesDePelicula(idCineSeleccionado, idPeliculaSeleccionada);

  ui->listaSesiones->clear();
  for(const auto& sesion : sesiones){
    std::time_t hora = sesion.getHoraInicio();
    std::stringstream ss;
    ss << std::put_time(std::localtime(&hora), "%d/%m/%Y %H:%M");
    std::string fechaLegible = ss.str();

    QString textoFila = "Sala " + QString::number(sesion.getIdSala()) + ": " + QString::fromStdString(fechaLegible);

    QListWidgetItem *item = new QListWidgetItem(textoFila);
    item->setData(Qt::UserRole, sesion.getId());
    ui->listaSesiones->addItem(item);
  }
  
  ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::alSeleccionarSesion(QListWidgetItem *item) {
  idSesionSeleccionada = item->data(Qt::UserRole).toInt();

  Sesion sesion = db.obtenerSesion(idSesionSeleccionada);
  Sala sala = db.obtenerSala(sesion.getIdSala());
  std::vector<Reserva> reservas = db.obtenerReservasDeSesion(idSesionSeleccionada);

  if (ui->containerSala->layout() != nullptr) {
    QLayoutItem *child;
    while ((child = ui->containerSala->layout()->takeAt(0)) != nullptr) {
      delete child->widget();
      delete child;
    }
    delete ui->containerSala->layout();
  }

  QGridLayout *grid = new QGridLayout(ui->containerSala);

  for (int f = 0; f < sala.getFilas(); ++f) {
    for (int c = 0; c < sala.getColumnas(); ++c) {
      
      QPushButton *boton = new QPushButton(QString::number(c + 1));

      boton->setProperty("fila", f);
      boton->setProperty("columna", c);

      bool ocupado = false;
      for (const auto& r : reservas) {
        if (r.getFila() == f && r.getColumna() == c) {
          ocupado = true;
          break;
        }
      }

      if (ocupado) {
        boton->setStyleSheet("background-color: red; color: white;");
        boton->setEnabled(false);
      } else {
        boton->setStyleSheet("background-color: green; color: white;");
      }

      connect(boton, &QPushButton::clicked, this, &MainWindow::alPulsarButaca);

      grid->addWidget(boton, f, c);
    }
  }

  ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::alPulsarButaca() {
  QPushButton *boton = qobject_cast<QPushButton*>(sender());
  if (boton) {
    int fila = boton->property("fila").toInt();
    int columna = boton->property("columna").toInt();

    Reserva reserva(-1, idSesionSeleccionada, fila, columna, "COMPRADO", std::time(nullptr));
    int idReserva = db.crearReserva(reserva);

    if (idReserva != -1) {
      Cine cine = db.obtenerCine(idCineSeleccionado);
      Pelicula peli = db.obtenerPelicula(idPeliculaSeleccionada);
      
      QString detalles = "Cine: " + QString::fromStdString(cine.getNombre()) + "\n" +
                         "Película: " + QString::fromStdString(peli.getTitulo()) + "\n" +
                         "Asiento: Fila " + QString::number(fila + 1) + ", Columna " + QString::number(columna + 1);
      
      ui->labelTicketDetalles->setText(detalles);

      ui->stackedWidget->setCurrentIndex(4);
    }
  }
}

void MainWindow::alPulsarInicio() {
  idCineSeleccionado = -1;
  idPeliculaSeleccionada = -1;
  idSesionSeleccionada = -1;
  
  ui->stackedWidget->setCurrentIndex(0);
}