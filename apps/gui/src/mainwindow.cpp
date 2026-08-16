#include "mainwindow.h"

#include <QFile>
#include <QMessageBox>
#include <map>

#include "cinecardwidget.h"
#include "logindialog.h"
#include "moviecardwidget.h"
#include "qrhelper.h"
#include "tarifasdialog.h"
#include "ui_mainwindow.h"
#include "utils/time_utils.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  api = new ApiClient(this);

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
  idSesionSeleccionada = -1;

  // Conexiones de la interfaz
  connect(ui->botonInicio, &QPushButton::clicked, this,
          &MainWindow::alPulsarInicio);

  connect(ui->botonAtrasPeliculas, &QPushButton::clicked, this,
          [this]() { ui->stackedWidget->setCurrentIndex(0); });
  connect(ui->botonAtrasSesiones, &QPushButton::clicked, this,
          [this]() { ui->stackedWidget->setCurrentIndex(1); });
  connect(ui->botonAtrasSala, &QPushButton::clicked, this,
          [this]() { ui->stackedWidget->setCurrentIndex(2); });

  connect(ui->inputBuscar, &QLineEdit::textChanged, this,
          &MainWindow::alFiltrarPeliculas);
  connect(ui->comboGenero, &QComboBox::currentTextChanged, this,
          &MainWindow::alFiltrarPeliculas);
  connect(ui->botonConfirmar, &QPushButton::clicked, this,
          &MainWindow::alConfirmarCompra);

  // Botón de usuario en la barra superior
  btnUsuario = new QPushButton(this);
  btnUsuario->setStyleSheet(
      "QPushButton { background-color: #1E202D; color: #E5C07B; font-weight: "
      "bold; border: 1px solid #2B2E3D; border-radius: 6px; padding: 6px 14px; "
      "} QPushButton:hover { background-color: #2B2E3D; color: #61AFEF; }");
  connect(btnUsuario, &QPushButton::clicked, this,
          &MainWindow::alPulsarBotonUsuario);
  ui->horizontalLayoutHeader->addWidget(btnUsuario);

  cargarCines();

  // Mostrar el LoginDialog al inicio
  LoginDialog loginInicial(api, this);
  loginInicial.exec();
  usuarioActual = loginInicial.getUsuarioObtenido();
  actualizarBotonUsuario();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::cargarCines() {
  api->obtenerCines(
      [this](bool ok, QList<Cine> cines) {
        if (!ok) return;
        listaCines = cines;
        ui->listaCines->clear();

        for (const auto& cine : cines) {
          QListWidgetItem* item = new QListWidgetItem(ui->listaCines);
          CineCardWidget* tarjeta = new CineCardWidget(cine, this);
          item->setSizeHint(tarjeta->sizeHint());
          ui->listaCines->addItem(item);
          ui->listaCines->setItemWidget(item, tarjeta);

          connect(tarjeta, &CineCardWidget::cineSeleccionado, this,
                  &MainWindow::alSeleccionarCine);
        }
      },
      this);
}

void MainWindow::alPulsarBotonUsuario() {
  LoginDialog loginDlg(api, this);
  if (loginDlg.exec() == QDialog::Accepted) {
    usuarioActual = loginDlg.getUsuarioObtenido();
    actualizarBotonUsuario();
  }
}

void MainWindow::actualizarBotonUsuario() {
  if (usuarioActual.esValido()) {
    btnUsuario->setText(QString("👤 %1 (%2)")
                            .arg(QString::fromStdString(usuarioActual.getNombre()))
                            .arg(QString::fromStdString(usuarioActual.getDni())));
  } else {
    btnUsuario->setText("👤 Iniciar Sesión");
  }
}

void MainWindow::alSeleccionarCine(int idCine) {
  idCineSeleccionado = idCine;

  ui->inputBuscar->blockSignals(true);
  ui->inputBuscar->clear();
  ui->inputBuscar->blockSignals(false);
  ui->comboGenero->blockSignals(true);
  ui->comboGenero->setCurrentIndex(0);
  ui->comboGenero->blockSignals(false);
  ui->labelNoResultados->hide();
  ui->listaPeliculas->show();

  // Cargar películas asíncronamente desde la API (solo cartelera del cine seleccionado)
  api->obtenerPeliculas(
      idCineSeleccionado,
      [this](bool ok, QList<Pelicula> peliculas) {
        if (!ok) return;
        listaPeliculas = peliculas;

        // Limpiar lista anterior
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
      },
      this);
}

void MainWindow::alSeleccionarPelicula(int idPelicula) {
  idPeliculaSeleccionada = idPelicula;

  Pelicula pelicula(-1, "", Genero::NONE, 0);
  for (const auto& p : listaPeliculas) {
    if (p.getId() == idPeliculaSeleccionada) {
      pelicula = p;
      break;
    }
  }

  QString rutaPoster = QString::fromStdString(
      "data/images/peliculas/" + std::to_string(pelicula.getId()) + ".jpg");
  if (!QFile::exists(rutaPoster)) {
    rutaPoster = "data/images/peliculas/default.jpg";
  }
  QPixmap pixmap(rutaPoster);
  ui->labelSesionPoster->setPixmap(pixmap.scaled(
      200, 270, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
  ui->labelSesionPoster->setFixedSize(200, 270);
  ui->labelSesionTitulo->setText(QString::fromStdString(pelicula.getTitulo()));

  QString infoStr =
      QString::fromStdString(generoToString(pelicula.getGenero())) + " • " +
      QString::number(pelicula.getDuracion()) + " min";
  ui->labelSesionInfo->setText(infoStr);

  // Cargar sesiones asíncronamente desde la API
  api->obtenerSesiones(
      idCineSeleccionado,
      [this, idPelicula](bool ok, QList<Sesion> sesiones) {
        if (!ok) return;
        listaSesiones = sesiones;

        if (ui->scrollAreaWidgetContentsSesiones->layout() != nullptr) {
          delete ui->scrollAreaWidgetContentsSesiones->layout();
        }
        const auto children =
            ui->scrollAreaWidgetContentsSesiones->findChildren<QWidget*>();
        for (QWidget* child : children) {
          child->deleteLater();
        }

        QVBoxLayout* scrollLayout =
            new QVBoxLayout(ui->scrollAreaWidgetContentsSesiones);
        scrollLayout->setAlignment(Qt::AlignTop);
        scrollLayout->setSpacing(15);
        scrollLayout->setContentsMargins(10, 10, 10, 10);

        std::map<std::string, std::pair<std::string, std::vector<Sesion>>>
            sesionesPorDia;
        for (const auto& sesion : sesiones) {
          if (sesion.getPelicula().getId() != idPelicula &&
              sesion.getPelicula().getId() != -1 && idPelicula != -1) {
            // Filtrar sesiones de la película seleccionada si la película viene tipada
            if (sesion.getPelicula().getId() != idPelicula) continue;
          }

          std::time_t hora = sesion.getHoraInicio();
          std::tm timeinfo = utils::safeLocalTime(hora);

          char keyBuf[20];
          std::strftime(keyBuf, sizeof(keyBuf), "%Y-%m-%d", &timeinfo);
          std::string key = keyBuf;

          char labelBuf[50];
          std::strftime(labelBuf, sizeof(labelBuf), "%A, %d de %B", &timeinfo);
          std::string readable = labelBuf;

          sesionesPorDia[key].first = readable;
          sesionesPorDia[key].second.push_back(sesion);
        }

        for (const auto& par : sesionesPorDia) {
          QLabel* labelDia =
              new QLabel(QString::fromStdString(par.second.first).toUpper(),
                         ui->scrollAreaWidgetContentsSesiones);
          labelDia->setStyleSheet(
              "font-weight: bold; font-size: 11px; color: #abb2bf; margin-top: "
              "10px;");
          scrollLayout->addWidget(labelDia);

          QHBoxLayout* layoutBotones = new QHBoxLayout();
          layoutBotones->setAlignment(Qt::AlignLeft);
          layoutBotones->setSpacing(10);

          for (const auto& sesion : par.second.second) {
            std::time_t hora = sesion.getHoraInicio();
            std::tm timeinfo = utils::safeLocalTime(hora);
            char timeBuf[10];
            std::strftime(timeBuf, sizeof(timeBuf), "%H:%M", &timeinfo);

            QString textoBoton =
                QString(timeBuf) + "\nSala " + QString::number(sesion.getIdSala());

            QPushButton* botonSesion =
                new QPushButton(textoBoton, ui->scrollAreaWidgetContentsSesiones);
            botonSesion->setFixedSize(90, 50);
            botonSesion->setProperty("idSesion", sesion.getId());
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
                "}");

            connect(botonSesion, &QPushButton::clicked, this,
                    &MainWindow::alPulsarBotonSesion);
            layoutBotones->addWidget(botonSesion);
          }
          scrollLayout->addLayout(layoutBotones);
        }

        ui->stackedWidget->setCurrentIndex(2);
      },
      this);
}

void MainWindow::alPulsarBotonSesion() {
  auto* boton = qobject_cast<QPushButton*>(sender());
  if (boton) {
    int idSesion = boton->property("idSesion").toInt();
    alSeleccionarSesionPorId(idSesion);
  }
}

void MainWindow::alSeleccionarSesionPorId(int idSession) {
  idSesionSeleccionada = idSession;
  butacasSeleccionadas.clear();
  int precioTotal = 0;

  if (ui->containerSala->layout() != nullptr) {
    delete ui->containerSala->layout();
  }
  const auto butacas = ui->containerSala->findChildren<QWidget*>();
  for (QWidget* b : butacas) {
    b->deleteLater();
  }

  // Dimensiones estándar de sala (5 filas x 7 columnas = 35 butacas)
  int filas = 5;
  int columnas = 7;

  int sizeW = 500 / columnas;
  int sizeH = 300 / filas;
  int butacaSize = std::min(sizeW, sizeH);
  butacaSize = std::clamp(butacaSize, 12, 35);

  QGridLayout* grid = new QGridLayout(ui->containerSala);
  grid->setSpacing(butacaSize > 20 ? 6 : 3);
  grid->setContentsMargins(0, 0, 0, 0);

  for (int f = 0; f < filas; ++f) {
    for (int c = 0; c < columnas; ++c) {
      QPushButton* boton = new QPushButton(ui->containerSala);
      boton->setFixedSize(butacaSize, butacaSize);
      boton->setObjectName("botonButaca");
      boton->setCheckable(true);
      boton->setToolTip("Fila " + QString::number(f + 1) + ", Asiento " +
                        QString::number(c + 1));
      boton->setProperty("fila", f);
      boton->setProperty("columna", c);

      connect(boton, &QPushButton::clicked, this, &MainWindow::alPulsarButaca);
      grid->addWidget(boton, f, c);
    }
  }

  ui->labelPrecioTotal->setText("Precio: " + QString::number(precioTotal) + "€");
  ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::alPulsarButaca() {
  auto* boton = qobject_cast<QPushButton*>(sender());
  if (boton) {
    int fila = boton->property("fila").toInt();
    int columna = boton->property("columna").toInt();
    std::pair<int, int> butaca = {fila, columna};

    if (boton->isChecked()) {
      butacasSeleccionadas.insert(butaca);
    } else {
      butacasSeleccionadas.erase(butaca);
    }

    double precioTotal = butacasSeleccionadas.size() * 7.50;
    ui->labelPrecioTotal->setText(
        "Total: " + QString::number(precioTotal, 'f', 2) + " €");
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
    auto* tarjeta = qobject_cast<MovieCardWidget*>(widget);

    if (tarjeta) {
      bool coincideTexto =
          textoBusqueda.isEmpty() ||
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

void MainWindow::alConfirmarCompra() {
  if (butacasSeleccionadas.empty()) return;

  if (!usuarioActual.esValido()) {
    LoginDialog loginDlg(api, this);
    if (loginDlg.exec() != QDialog::Accepted || !loginDlg.estaLogueado()) {
      return;
    }
    usuarioActual = loginDlg.getUsuarioObtenido();
    actualizarBotonUsuario();
  }

  TarifasDialog dialog(butacasSeleccionadas, this);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  auto tarifasElegidas = dialog.getTarifasSeleccionadas();
  float totalCompra = dialog.getPrecioTotal();

  QList<Reserva> reservasParaCrear;
  for (const auto& t : tarifasElegidas) {
    reservasParaCrear.append(Reserva(-1, idSesionSeleccionada, t.fila, t.columna,
                                     "COMPRADO", std::time(nullptr), t.tipo,
                                     t.precio));
  }

  // Realizar reserva atómica asíncrona a través de ApiClient
  api->crearReservas(
      idSesionSeleccionada, reservasParaCrear,
      [this, tarifasElegidas, totalCompra](bool ok, QString errorMsg) {
        if (!ok) {
          QMessageBox::warning(this, "Error de Reserva", errorMsg);
          return;
        }

        Cine cine(1, "Cine Central", "Gran Vía");
        for (const auto& c : listaCines) {
          if (c.getId() == idCineSeleccionado) {
            cine = c;
            break;
          }
        }

        Pelicula peli(1, "Película", Genero::ACCION, 120);
        for (const auto& p : listaPeliculas) {
          if (p.getId() == idPeliculaSeleccionada) {
            peli = p;
            break;
          }
        }

        Sesion sesion(idSesionSeleccionada, peli, 1, std::time(nullptr));
        for (const auto& s : listaSesiones) {
          if (s.getId() == idSesionSeleccionada) {
            sesion = s;
            break;
          }
        }

        QString asientosStr;
        for (const auto& t : tarifasElegidas) {
          if (!asientosStr.isEmpty()) {
            asientosStr += ", ";
          }
          asientosStr += QString("F%1-A%2 (%3)")
                             .arg(t.fila + 1)
                             .arg(t.columna + 1)
                             .arg(QString::fromStdString(t.tipo));
        }

        std::time_t hora = sesion.getHoraInicio();
        std::tm timeinfo = utils::safeLocalTime(hora);
        char dateBuf[64];
        std::strftime(dateBuf, sizeof(dateBuf), "%A, %d de %B - %H:%M", &timeinfo);
        QString fechaHoraStr = QString::fromStdString(dateBuf).toUpper();

        QString clienteNombre = QString::fromStdString(
            usuarioActual.getNombre() + " " + usuarioActual.getApellidos());
        QString clienteDni = QString::fromStdString(usuarioActual.getDni());

        QString qrPayload =
            QString(
                "CINEMANAGER TICKET\n"
                "Cliente: %1 (%2)\n"
                "Pelicula: %3\n"
                "Cine: %4\n"
                "Sala: Sala %5\n"
                "Sesion: %6\n"
                "Asientos: %7\n"
                "Total: %8 €")
                .arg(clienteNombre)
                .arg(clienteDni)
                .arg(QString::fromStdString(peli.getTitulo()))
                .arg(QString::fromStdString(cine.getNombre()))
                .arg(sesion.getIdSala())
                .arg(fechaHoraStr)
                .arg(asientosStr)
                .arg(QString::number(totalCompra, 'f', 2));

        QPixmap qrPixmap = QrHelper::generarQR(qrPayload, 180);
        ui->labelQR->setPixmap(qrPixmap);
        ui->labelQR->setFixedSize(180, 180);

        QString detallesHTML =
            QString(
                "<html><body>"
                "<p style='font-size:11px; font-weight:bold; color:#00f0b5; "
                "margin-bottom:4px; letter-spacing:1px;'>TICKET DE ENTRADA</p>"
                "<p style='font-size:16px; font-weight:bold; color:#ffffff; "
                "margin:0;'>%1</p>"
                "<p style='font-size:11px; color:#abb2bf; margin-top:2px;'>%2 • %3 "
                "min</p>"
                "<hr style='border: 0; border-top: 1px dashed #3e4452; margin: 8px "
                "0;'>"
                "<table cellspacing='4' style='color:#ffffff; font-size:11px; "
                "font-family: sans-serif;'>"
                "<tr><td style='color:#abb2bf; font-weight:bold; "
                "width:75px;'>TITULAR:</td><td>%4 (%5)</td></tr>"
                "<tr><td style='color:#abb2bf; font-weight:bold;'>CINE:</td><td>%6</td></tr>"
                "<tr><td style='color:#abb2bf; "
                "font-weight:bold;'>SALA:</td><td>Sala %7</td></tr>"
                "<tr><td style='color:#abb2bf; "
                "font-weight:bold;'>SESIÓN:</td><td>%8</td></tr>"
                "<tr><td style='color:#abb2bf; "
                "font-weight:bold;'>ASIENTOS:</td><td>%9</td></tr>"
                "</table>"
                "<hr style='border: 0; border-top: 1px dashed #3e4452; margin: 8px "
                "0;'>"
                "<p style='font-size:14px; font-weight:bold; color:#00f0b5; "
                "margin:0;'>TOTAL COMPRA: %10</p>"
                "</body></html>")
                .arg(QString::fromStdString(peli.getTitulo()))
                .arg(QString::fromStdString(generoToString(peli.getGenero())))
                .arg(peli.getDuracion())
                .arg(clienteNombre)
                .arg(clienteDni)
                .arg(QString::fromStdString(cine.getNombre()))
                .arg(sesion.getIdSala())
                .arg(fechaHoraStr)
                .arg(asientosStr)
                .arg(QString::number(totalCompra, 'f', 2) + " €");

        ui->labelTicketDetalles->setText(detallesHTML);
        ui->stackedWidget->setCurrentIndex(4);
      },
      this);
}