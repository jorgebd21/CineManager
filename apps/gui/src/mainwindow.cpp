#include "../include/mainwindow.h"
#include <map>
#include <QFile>
#include "cinecardwidget.h"
#include "moviecardwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ui->listaPeliculas->setViewMode(QListView::IconMode);
  ui->listaPeliculas->setResizeMode(QListView::Adjust);
  ui->listaPeliculas->setFlow(QListView::LeftToRight);
  ui->listaPeliculas->setWrapping(true);
  ui->listaPeliculas->setSpacing(20);
  
  ui->listaCines->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  ui->listaCines->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  ui->listaCines->verticalScrollBar()->setSingleStep(15);
  ui->listaPeliculas->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  ui->listaPeliculas->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  ui->listaPeliculas->verticalScrollBar()->setSingleStep(15);
  QScroller::grabGesture(ui->listaPeliculas, QScroller::LeftMouseButtonGesture);

  ui->comboGenero->addItem("Todos los géneros");
  ui->comboGenero->addItem("ACCION");
  ui->comboGenero->addItem("DRAMA");
  ui->comboGenero->addItem("CIENCIA_FICCION");
  ui->comboGenero->addItem("COMEDIA");
  ui->comboGenero->addItem("TERROR");

  ui->labelNoResultados->hide();

  idCineSeleccionado = -1;
  idPeliculaSeleccionada = -1;

  std::vector<Cine> cines = db.obtenerCines();

  for (const auto& cine : cines) {
    QListWidgetItem* item = new QListWidgetItem(ui->listaCines);

    CineCardWidget* tarjeta = new CineCardWidget(cine, this);

    item->setSizeHint(tarjeta->sizeHint());

    ui->listaCines->addItem(item);
    ui->listaCines->setItemWidget(item, tarjeta);

    connect(tarjeta, &CineCardWidget::cineSeleccionado, this,
            &MainWindow::alSeleccionarCine);
  }

  // Conexiones de la interfaz
  connect(ui->botonInicio, &QPushButton::clicked, this,
          &MainWindow::alPulsarInicio);

  // Conexiones de botones de navegación Atrás
  connect(ui->botonAtrasPeliculas, &QPushButton::clicked, this, [this]() {
    ui->stackedWidget->setCurrentIndex(0);
  });
  connect(ui->botonAtrasSesiones, &QPushButton::clicked, this, [this]() {
    ui->stackedWidget->setCurrentIndex(1);
  });
  connect(ui->botonAtrasSala, &QPushButton::clicked, this, [this]() {
    ui->stackedWidget->setCurrentIndex(2);
  });

  connect(ui->inputBuscar, &QLineEdit::textChanged, this, &MainWindow::alFiltrarPeliculas);
  
  connect(ui->comboGenero, &QComboBox::currentTextChanged, this, &MainWindow::alFiltrarPeliculas);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::alSeleccionarCine(int idCine) {
  idCineSeleccionado = idCine;

  // Limpiar filtros al seleccionar un nuevo cine sin disparar señales intermedias
  ui->inputBuscar->blockSignals(true);
  ui->inputBuscar->clear();
  ui->inputBuscar->blockSignals(false);
  ui->comboGenero->blockSignals(true);
  ui->comboGenero->setCurrentIndex(0);
  ui->comboGenero->blockSignals(false);
  ui->labelNoResultados->hide();
  ui->listaPeliculas->show();

  std::vector<Pelicula> peliculas = db.obtenerCartelera(idCineSeleccionado);

  // Desvincular y destruir las tarjetas de películas de forma asíncrona para evitar use-after-free
  for (int i = 0; i < ui->listaPeliculas->count(); ++i) {
    QListWidgetItem* item = ui->listaPeliculas->item(i);
    QWidget* widget = ui->listaPeliculas->itemWidget(item);
    if (widget) {
      ui->listaPeliculas->removeItemWidget(item);
      widget->deleteLater();
    }
  }
  ui->listaPeliculas->clear();

  for (const auto& pelicula : peliculas) {
    QListWidgetItem* item = new QListWidgetItem(ui->listaPeliculas);
    MovieCardWidget* tarjeta = new MovieCardWidget(pelicula, this);
    item->setSizeHint(tarjeta->sizeHint());
    ui->listaPeliculas->addItem(item);
    ui->listaPeliculas->setItemWidget(item, tarjeta);
    connect(tarjeta, &MovieCardWidget::PeliculaSeleccionada, this,
            &MainWindow::alSeleccionarPelicula);
  }

  ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::alSeleccionarPelicula(int idPelicula) {
  idPeliculaSeleccionada = idPelicula;

  // A. Cargar datos e imagen de la película seleccionada a la izquierda
  Pelicula pelicula = db.obtenerPelicula(idPeliculaSeleccionada);
  QString rutaPoster = QString::fromStdString("data/images/peliculas/" + std::to_string(pelicula.getId()) + ".jpg");
  if (!QFile::exists(rutaPoster)) {
    rutaPoster = "data/images/peliculas/default.jpg";
  }
  QPixmap pixmap(rutaPoster);
  ui->labelSesionPoster->setPixmap(pixmap.scaled(200, 270, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
  ui->labelSesionPoster->setFixedSize(200, 270);
  ui->labelSesionTitulo->setText(QString::fromStdString(pelicula.getTitulo()));
  
  QString infoStr = QString::fromStdString(generoToString(pelicula.getGenero())) + " • " + QString::number(pelicula.getDuracion()) + " min";
  ui->labelSesionInfo->setText(infoStr);

  // B. Cargar las sesiones agrupadas por día a la derecha
  std::vector<Sesion> sesiones = db.obtenerSesionesDePelicula(idCineSeleccionado, idPeliculaSeleccionada);

  // Limpiar el layout anterior y widgets del scrollArea de forma segura usando deleteLater()
  if (ui->scrollAreaWidgetContentsSesiones->layout() != nullptr) {
    delete ui->scrollAreaWidgetContentsSesiones->layout();
  }
  const auto children = ui->scrollAreaWidgetContentsSesiones->findChildren<QWidget*>();
  for (QWidget* child : children) {
    child->deleteLater();
  }

  QVBoxLayout* scrollLayout = new QVBoxLayout(ui->scrollAreaWidgetContentsSesiones);
  scrollLayout->setAlignment(Qt::AlignTop);
  scrollLayout->setSpacing(15);
  scrollLayout->setContentsMargins(10, 10, 10, 10);

  // Agrupar sesiones por día (YYYY-MM-DD)
  std::map<std::string, std::pair<std::string, std::vector<Sesion>>> sesionesPorDia;
  for (const auto& sesion : sesiones) {
    std::time_t hora = sesion.getHoraInicio();
    std::tm* timeinfo = std::localtime(&hora);
    
    char keyBuf[20];
    std::strftime(keyBuf, sizeof(keyBuf), "%Y-%m-%d", timeinfo);
    std::string key = keyBuf;

    char labelBuf[50];
    std::strftime(labelBuf, sizeof(labelBuf), "%A, %d de %B", timeinfo);
    std::string readable = labelBuf;

    sesionesPorDia[key].first = readable;
    sesionesPorDia[key].second.push_back(sesion);
  }

  // Rellenar dinámicamente con etiquetas de día y botones horizontales de sesiones
  for (const auto& par : sesionesPorDia) {
    QLabel* labelDia = new QLabel(QString::fromStdString(par.second.first).toUpper(), ui->scrollAreaWidgetContentsSesiones);
    labelDia->setStyleSheet("font-weight: bold; font-size: 11px; color: #abb2bf; margin-top: 10px;");
    scrollLayout->addWidget(labelDia);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    layoutBotones->setAlignment(Qt::AlignLeft);
    layoutBotones->setSpacing(10);

    for (const auto& sesion : par.second.second) {
      std::time_t hora = sesion.getHoraInicio();
      std::tm* timeinfo = std::localtime(&hora);
      char timeBuf[10];
      std::strftime(timeBuf, sizeof(timeBuf), "%H:%M", timeinfo);

      QString textoBoton = QString(timeBuf) + "\nSala " + QString::number(sesion.getIdSala());
      QPushButton* botonSesion = new QPushButton(textoBoton, ui->scrollAreaWidgetContentsSesiones);
      botonSesion->setFixedSize(90, 50);
      botonSesion->setProperty("idSesion", sesion.getId());
      
      // Estilo de los botones de la sesión
      botonSesion->setStyleSheet(
          "QPushButton {"
          "  background-color: #1e222b;"
          "  border: 1px solid #3e4452;"
          "  border-radius: 8px;"
          "  color: #ffffff;"
          "  font-weight: bold;"
          "  font-size: 11px;"
          "}"
          "QPushButton:hover {"
          "  background-color: #00f0b5;"
          "  color: #121418;"
          "  border: 1px solid #00f0b5;"
          "}"
      );

      connect(botonSesion, &QPushButton::clicked, this, &MainWindow::alPulsarBotonSesion);
      layoutBotones->addWidget(botonSesion);
    }
    scrollLayout->addLayout(layoutBotones);
  }

  ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::alPulsarBotonSesion() {
  QPushButton* boton = qobject_cast<QPushButton*>(sender());
  if (boton) {
    int idSesion = boton->property("idSesion").toInt();
    alSeleccionarSesionPorId(idSesion);
  }
}

void MainWindow::alSeleccionarSesionPorId(int idSession) {
  idSesionSeleccionada = idSession;

  Sesion sesion = db.obtenerSesion(idSesionSeleccionada);
  Sala sala = db.obtenerSala(sesion.getIdSala());
  std::vector<Reserva> reservas = db.obtenerReservasDeSesion(idSesionSeleccionada);

  // Limpiar el layout anterior y butacas viejas de forma segura con deleteLater()
  if (ui->containerSala->layout() != nullptr) {
    delete ui->containerSala->layout();
  }
  const auto butacas = ui->containerSala->findChildren<QWidget*>();
  for (QWidget* b : butacas) {
    b->deleteLater();
  }

  int filas = sala.getFilas();
  int columnas = sala.getColumnas();
  int totalButacas = filas * columnas;

  // En caso de que el aforo sea absurdo, mostramos aviso de límite
  if (totalButacas > 500) {
    QLabel* labelAviso = new QLabel("El aforo de esta sala supera el límite permitido de 500 butacas para venta online.", ui->containerSala);
    labelAviso->setStyleSheet("color: #e74c3c; font-size: 14px; font-weight: bold;");
    labelAviso->setAlignment(Qt::AlignCenter);
    labelAviso->setWordWrap(true);
    
    QVBoxLayout* avisoLayout = new QVBoxLayout(ui->containerSala);
    avisoLayout->addWidget(labelAviso);
    
    ui->labelPrecioTotal->setText("Precio: 7.50 €");
    ui->stackedWidget->setCurrentIndex(3);
    return;
  }

  // Calcular tamaño dinámico de las butacas (contenedor de aprox 500x300)
  int sizeW = 500 / columnas;
  int sizeH = 300 / filas;
  int butacaSize = std::min(sizeW, sizeH);
  
  // Acotar el tamaño para que no queden ni gigantes ni microscópicas
  butacaSize = std::clamp(butacaSize, 12, 35);

  QGridLayout* grid = new QGridLayout(ui->containerSala);
  // Si la sala es pequeña, más espacio; si es densa, espacio muy fino
  grid->setSpacing(butacaSize > 20 ? 6 : 3);
  grid->setContentsMargins(0, 0, 0, 0);

  for (int f = 0; f < filas; ++f) {
    for (int c = 0; c < columnas; ++c) {
      QPushButton* boton = new QPushButton(ui->containerSala);
      boton->setFixedSize(butacaSize, butacaSize);
      boton->setObjectName("botonButaca");
      
      // Tooltip informativo sobre la fila y el asiento
      boton->setToolTip("Fila " + QString::number(f + 1) + ", Asiento " + QString::number(c + 1));

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
        boton->setEnabled(false);
      }

      connect(boton, &QPushButton::clicked, this, &MainWindow::alPulsarButaca);
      grid->addWidget(boton, f, c);
    }
  }

  // Mostrar el precio de la sesión en la leyenda inferior de la sala (precio fijo para el MVP de la GUI)
  ui->labelPrecioTotal->setText("Precio: 7.50 €");

  ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::alPulsarButaca() {
  QPushButton* boton = qobject_cast<QPushButton*>(sender());
  if (boton) {
    int fila = boton->property("fila").toInt();
    int columna = boton->property("columna").toInt();

    Reserva reserva(-1, idSesionSeleccionada, fila, columna, "COMPRADO",
                    std::time(nullptr));
    int idReserva = db.crearReserva(reserva);

    if (idReserva != -1) {
      Cine cine = db.obtenerCine(idCineSeleccionado);
      Pelicula peli = db.obtenerPelicula(idPeliculaSeleccionada);
      Sesion sesion = db.obtenerSesion(idSesionSeleccionada);

      // Cargar y escalar la imagen del código QR real
      QPixmap qrPixmap("data/images/qr_mock.jpg");
      ui->labelQR->setPixmap(qrPixmap.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      ui->labelQR->setFixedSize(180, 180);

      // Formatear la fecha y hora legible
      std::time_t hora = sesion.getHoraInicio();
      std::tm* timeinfo = std::localtime(&hora);
      char dateBuf[64];
      std::strftime(dateBuf, sizeof(dateBuf), "%A, %d de %B - %H:%M", timeinfo);
      QString fechaHoraStr = QString::fromStdString(dateBuf).toUpper();

      // Formatear los detalles de compra en un HTML de ticket clásico de cine
      QString detallesHTML = QString(
          "<html><body>"
          "<p style='font-size:11px; font-weight:bold; color:#00f0b5; margin-bottom:4px; letter-spacing:1px;'>TICKET DE ENTRADA</p>"
          "<p style='font-size:16px; font-weight:bold; color:#ffffff; margin:0;'>%1</p>"
          "<p style='font-size:11px; color:#abb2bf; margin-top:2px;'>%2 • %3 min</p>"
          "<hr style='border: 0; border-top: 1px dashed #3e4452; margin: 8px 0;'>"
          "<table cellspacing='4' style='color:#ffffff; font-size:11px; font-family: sans-serif;'>"
          "<tr><td style='color:#abb2bf; font-weight:bold; width:70px;'>CINE:</td><td>%4</td></tr>"
          "<tr><td style='color:#abb2bf; font-weight:bold;'>SALA:</td><td>Sala %5</td></tr>"
          "<tr><td style='color:#abb2bf; font-weight:bold;'>SESIÓN:</td><td>%6</td></tr>"
          "<tr><td style='color:#abb2bf; font-weight:bold;'>ASIENTO:</td><td>Fila %7, Asiento %8</td></tr>"
          "</table>"
          "<hr style='border: 0; border-top: 1px dashed #3e4452; margin: 8px 0;'>"
          "<p style='font-size:14px; font-weight:bold; color:#00f0b5; margin:0;'>TOTAL COMPRA: 7.50 €</p>"
          "</body></html>"
      )
      .arg(QString::fromStdString(peli.getTitulo()))
      .arg(QString::fromStdString(generoToString(peli.getGenero())))
      .arg(peli.getDuracion())
      .arg(QString::fromStdString(cine.getNombre()))
      .arg(sesion.getIdSala())
      .arg(fechaHoraStr)
      .arg(fila + 1)
      .arg(columna + 1);

      ui->labelTicketDetalles->setText(detallesHTML);

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

void MainWindow::alFiltrarPeliculas() {
  QString textoBusqueda = ui->inputBuscar->text().trimmed();
  QString generoSeleccionado = ui->comboGenero->currentText();
  int visibles = 0;

  for (int i = 0; i < ui->listaPeliculas->count(); ++i) {
    QListWidgetItem* item = ui->listaPeliculas->item(i);
    QWidget* widget = ui->listaPeliculas->itemWidget(item);
    MovieCardWidget* tarjeta = qobject_cast<MovieCardWidget*>(widget);

    if (tarjeta) {
      bool coincideTexto = textoBusqueda.isEmpty() || 
                           tarjeta->getTitulo().contains(textoBusqueda, Qt::CaseInsensitive);

      bool coincideGenero = (generoSeleccionado == "Todos los géneros") || 
                             (tarjeta->getGenero() == generoSeleccionado);

      bool ocultar = !(coincideTexto && coincideGenero);
      item->setHidden(ocultar);
      if (!ocultar) {
        visibles++;
      }
    }
  }

  if (visibles == 0) {
    ui->labelNoResultados->show();
    ui->listaPeliculas->hide();
  } else {
    ui->labelNoResultados->hide();
    ui->listaPeliculas->show();
  }
}