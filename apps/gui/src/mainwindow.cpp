#include "mainwindow.h"

#include <QFile>
#include <QMessageBox>
#include <QStatusBar>
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

  // Cargar imagen de fondo con olas en las esquinas
  std::vector<QString> posiblesFondos = {
      "data/images/background.jpg",
      "../data/images/background.jpg",
      "../../data/images/background.jpg",
      QApplication::applicationDirPath() + "/data/images/background.jpg",
      QApplication::applicationDirPath() + "/../data/images/background.jpg",
      "/home/jorgebd/Documentos/Proyectos/CineManager/data/images/background.jpg"
  };
  for (const auto& ruta : posiblesFondos) {
    if (QFile::exists(ruta)) {
      pixmapFondoOriginal.load(ruta);
      break;
    }
  }
  actualizarFondo();

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

  // Conectar señal de servidor desconectado para notificaciones
  connect(api, &ApiClient::servidorDesconectado, this, [this](const QString& msg) {
    if (statusBar()) {
      statusBar()->showMessage("⚠️ " + msg, 10000);
    }
  });

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

void MainWindow::actualizarFondo() {
  if (pixmapFondoOriginal.isNull()) return;
  QPixmap scaled = pixmapFondoOriginal.scaled(
      this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
  QPalette pal = this->palette();
  pal.setBrush(QPalette::Window, QBrush(scaled));
  this->setPalette(pal);
  this->setAutoFillBackground(true);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
  QMainWindow::resizeEvent(event);
  actualizarFondo();
}

void MainWindow::cargarCines() {
  api->obtenerCines(
      [this](bool ok, QList<Cine> cines, const QString& errorMsg) {
        if (!ok) {
          QString detalle = errorMsg.isEmpty()
                                ? "No se pudo conectar con el servidor de la API."
                                : errorMsg;
          QMessageBox::critical(
              this, "Servidor de API No Disponible",
              "⚠️ No se pudo establecer conexión con el servidor de CineManager (" +
                  api->getBaseUrl() + ").\n\n" +
                  detalle + "\n\n"
                  "Por favor, verifique que el servicio de la API esté iniciado (vía Docker o CineManagerServer).");
          return;
        }
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
    btnUsuario->setText(QString("%1 (%2)")
                            .arg(QString::fromStdString(usuarioActual.getNombre()))
                            .arg(QString::fromStdString(usuarioActual.getDni())));
  } else {
    btnUsuario->setText("Iniciar Sesión");
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
      [this](bool ok, QList<Pelicula> peliculas, const QString& errorMsg) {
        if (!ok) {
          QString detalle = errorMsg.isEmpty()
                                ? "No se pudieron obtener las películas del servidor."
                                : errorMsg;
          QMessageBox::warning(this, "Servidor Inaccesible",
                               "⚠️ " + detalle + "\nCompruebe que el servidor API esté en ejecución.");
          return;
        }
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
      240, 330, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
  ui->labelSesionPoster->setFixedSize(240, 330);
  ui->labelSesionTitulo->setText(QString::fromStdString(pelicula.getTitulo()));

  QString infoStr =
      QString::fromStdString(generoToString(pelicula.getGenero())) + " • " +
      QString::number(pelicula.getDuracion()) + " min";
  ui->labelSesionInfo->setText(infoStr);

  // Cargar sesiones asíncronamente desde la API
  api->obtenerSesiones(
      idCineSeleccionado,
      [this, idPelicula](bool ok, QList<Sesion> sesiones, const QString& errorMsg) {
        if (!ok) {
          QString detalle = errorMsg.isEmpty()
                                ? "No se pudieron obtener las sesiones del servidor."
                                : errorMsg;
          QMessageBox::warning(this, "Servidor Inaccesible",
                               "⚠️ " + detalle + "\nCompruebe que el servidor API esté en ejecución.");
          return;
        }
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
        scrollLayout->setSpacing(14);
        scrollLayout->setContentsMargins(4, 4, 4, 4);

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
          std::string readable = utils::formatearFechaEspanol(hora, false);

          sesionesPorDia[key].first = readable;
          sesionesPorDia[key].second.push_back(sesion);
        }

        for (const auto& par : sesionesPorDia) {
          QLabel* labelDia =
              new QLabel(QString::fromStdString(par.second.first).toUpper(),
                         ui->scrollAreaWidgetContentsSesiones);
          labelDia->setStyleSheet(
              "font-weight: bold; font-size: 12px; color: #E5C07B; margin-top: "
              "8px; margin-bottom: 4px; letter-spacing: 0.5px;");
          scrollLayout->addWidget(labelDia);

          QHBoxLayout* layoutBotones = new QHBoxLayout();
          layoutBotones->setAlignment(Qt::AlignLeft);
          layoutBotones->setSpacing(12);

          for (const auto& sesion : par.second.second) {
            std::time_t hora = sesion.getHoraInicio();
            std::tm timeinfo = utils::safeLocalTime(hora);
            char timeBuf[10];
            std::strftime(timeBuf, sizeof(timeBuf), "%H:%M", &timeinfo);

            QString textoBoton =
                QString("%1\nSala %2 • 7.50 €")
                    .arg(timeBuf)
                    .arg(sesion.getIdSala());

            QPushButton* botonSesion =
                new QPushButton(textoBoton, ui->scrollAreaWidgetContentsSesiones);
            botonSesion->setFixedSize(140, 65);
            botonSesion->setProperty("idSesion", sesion.getId());
            botonSesion->setStyleSheet(
                "QPushButton {"
                "  background-color: #1E202D;"
                "  border: 1px solid #2B2E3D;"
                "  border-radius: 10px;"
                "  color: #FFFFFF;"
                "  font-weight: bold;"
                "  font-size: 11px;"
                "  padding: 6px;"
                "}"
                "QPushButton:hover {"
                "  background-color: #25293A;"
                "  color: #E5C07B;"
                "  border: 1px solid #E5C07B;"
                "}");

            connect(botonSesion, &QPushButton::clicked, this,
                    &MainWindow::alPulsarBotonSesion);
            layoutBotones->addWidget(botonSesion);
          }
          scrollLayout->addLayout(layoutBotones);
        }

        scrollLayout->addStretch();

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

  if (ui->containerSala->layout() != nullptr) {
    delete ui->containerSala->layout();
  }
  const auto butacas = ui->containerSala->findChildren<QWidget*>();
  for (QWidget* b : butacas) {
    b->deleteLater();
  }

  int idSala = 1;
  for (const auto& s : listaSesiones) {
    if (s.getId() == idSession) {
      idSala = s.getIdSala();
      break;
    }
  }

  api->obtenerSala(
      idSala,
      [this, idSession](bool okSala, Sala sala) {
        int filas = okSala && sala.getFilas() > 0 ? sala.getFilas() : 5;
        int columnas = okSala && sala.getColumnas() > 0 ? sala.getColumnas() : 7;

        api->obtenerReservasDeSesion(
            idSession,
            [this, filas, columnas](bool okRes, QList<Reserva> reservas) {
              std::set<std::pair<int, int>> ocupadas;
              if (okRes) {
                for (const auto& r : reservas) {
                  if (r.getEstado() == "COMPRADO" || r.getEstado() == "PENDIENTE") {
                    ocupadas.insert({r.getFila(), r.getColumna()});
                  }
                }
              }

              if (ui->containerSala->layout() != nullptr) {
                delete ui->containerSala->layout();
              }
              const auto prevButacas = ui->containerSala->findChildren<QWidget*>();
              for (QWidget* b : prevButacas) {
                b->deleteLater();
              }

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

                  bool estaOcupada = (ocupadas.find({f, c}) != ocupadas.end());
                  if (estaOcupada) {
                    boton->setEnabled(false);
                    boton->setStyleSheet(
                        "background-color: #E06C75; color: #1E202D; border: none; font-weight: bold; border-radius: 4px;");
                    boton->setToolTip("Fila " + QString::number(f + 1) + ", Asiento " +
                                      QString::number(c + 1) + " (Ocupado)");
                  } else {
                    boton->setCheckable(true);
                    boton->setToolTip("Fila " + QString::number(f + 1) + ", Asiento " +
                                      QString::number(c + 1));
                    boton->setProperty("fila", f);
                    boton->setProperty("columna", c);
                    connect(boton, &QPushButton::clicked, this, &MainWindow::alPulsarButaca);
                  }
                  grid->addWidget(boton, f, c);
                }
              }

              ui->labelPrecioTotal->setText("Precio: 0.00 €");
              ui->stackedWidget->setCurrentIndex(3);
            },
            this);
      },
      this);
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
        QString fechaHoraStr =
            QString::fromStdString(utils::formatearFechaEspanol(hora, true)).toUpper();

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

        QPixmap qrPixmap = QrHelper::generarQR(qrPayload, 240);
        ui->labelQR->setPixmap(qrPixmap);
        ui->labelQR->setFixedSize(240, 240);

        QString detallesHTML =
            QString(
                "<html><body style='font-family: sans-serif; color: #FFFFFF;'>"
                "<p style='font-size: 11px; font-weight: bold; color: #E5C07B; "
                "margin: 0; letter-spacing: 1.5px;'>ENTRADA DIGITAL OFICIAL</p>"
                "<h1 style='font-size: 19px; font-weight: bold; color: #FFFFFF; "
                "margin: 4px 0 2px 0;'>%1</h1>"
                "<p style='font-size: 12px; color: #61AFEF; margin: 0 0 8px 0; font-weight: bold;'>%2  •  %3 min</p>"
                "<hr style='border: 0; border-top: 1px dashed #3E4452; margin: 6px 0;'>"
                "<table cellpadding='3' cellspacing='0' style='width: 100%; color: #FFFFFF; font-size: 11px;'>"
                "<tr><td style='color: #8C91A4; font-weight: bold; width: 85px;'>TITULAR:</td><td><b>%4</b> <span style='color:#8C91A4;'>(%5)</span></td></tr>"
                "<tr><td style='color: #8C91A4; font-weight: bold;'>COMPLEJO:</td><td>%6</td></tr>"
                "<tr><td style='color: #8C91A4; font-weight: bold;'>SALA:</td><td><b>Sala %7</b></td></tr>"
                "<tr><td style='color: #8C91A4; font-weight: bold;'>HORARIO:</td><td><span style='color: #E5C07B; font-weight: bold;'>%8</span></td></tr>"
                "<tr><td style='color: #8C91A4; font-weight: bold;'>ASIENTOS:</td><td><b style='color: #10B981;'>%9</b></td></tr>"
                "</table>"
                "<hr style='border: 0; border-top: 1px dashed #3E4452; margin: 6px 0;'>"
                "<p style='font-size: 15px; font-weight: bold; color: #E5C07B; "
                "margin: 2px 0 0 0;'>TOTAL PAGADO: %10</p>"
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