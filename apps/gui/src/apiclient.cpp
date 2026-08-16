#include "apiclient.h"

ApiClient::ApiClient(QObject* parent, const QString& baseUrl)
    : QObject(parent),
      manager(new QNetworkAccessManager(this)),
      baseUrl(baseUrl) {}

void ApiClient::setBaseUrl(const QString& url) { baseUrl = url; }
QString ApiClient::getBaseUrl() const { return baseUrl; }

QString ApiClient::diagnosticarErrorRed(QNetworkReply* reply, const QString& baseUrl) {
  if (!reply) {
    return "Error de conexión desconocido con el servidor de la API.";
  }

  switch (reply->error()) {
    case QNetworkReply::ConnectionRefusedError:
      return QString("Conexión rechazada: El servidor de la API (%1) no está en ejecución o está caído.").arg(baseUrl);
    case QNetworkReply::HostNotFoundError:
      return QString("Servidor no encontrado: No se pudo localizar el host en %1.").arg(baseUrl);
    case QNetworkReply::TimeoutError:
      return QString("Tiempo de espera agotado: El servidor en %1 no responde.").arg(baseUrl);
    case QNetworkReply::RemoteHostClosedError:
      return "La conexión fue cerrada inesperadamente por el servidor de la API.";
    case QNetworkReply::ServiceUnavailableError:
      return "Servicio no disponible (HTTP 503): El servidor de la API está temporalmente fuera de servicio.";
    default:
      if (!reply->errorString().isEmpty()) {
        return QString("Error de comunicación con la API: %1").arg(reply->errorString());
      }
      return "Error de comunicación con el servidor de la API.";
  }
}

void ApiClient::healthCheck(std::function<void(bool, QString)> callback,
                            QObject* context) {
  QUrl url(baseUrl + "/api/v1/health");
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [this, reply, callback]() {
    bool ok = (reply->error() == QNetworkReply::NoError);
    QString errorMsg;
    if (!ok) {
      errorMsg = diagnosticarErrorRed(reply, baseUrl);
      emit servidorDesconectado(errorMsg);
    }
    callback(ok, errorMsg);
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::healthCheck(std::function<void(bool)> callback,
                            QObject* context) {
  healthCheck([callback](bool ok, const QString&) { callback(ok); }, context);
}

void ApiClient::obtenerCines(
    std::function<void(bool, QList<Cine>, QString)> callback,
    QObject* context) {
  QUrl url(baseUrl + "/api/v1/cines");
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [this, reply, callback]() {
    QList<Cine> lista;
    if (reply->error() != QNetworkReply::NoError) {
      QString errorMsg = diagnosticarErrorRed(reply, baseUrl);
      emit servidorDesconectado(errorMsg);
      callback(false, lista, errorMsg);
      reply->deleteLater();
      return;
    }

    QByteArray body = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject obj = doc.object();
    QJsonArray arr = obj["cines"].toArray();

    for (const QJsonValue& val : arr) {
      QJsonObject cObj = val.toObject();
      Cine c(cObj["id"].toInt(), cObj["nombre"].toString().toStdString(),
             cObj["direccion"].toString().toStdString());
      lista.append(c);
    }
    callback(true, lista, "");
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::obtenerCines(std::function<void(bool, QList<Cine>)> callback,
                             QObject* context) {
  obtenerCines([callback](bool ok, QList<Cine> cines, const QString&) {
    callback(ok, std::move(cines));
  }, context);
}

void ApiClient::obtenerPeliculas(
    int idCine, std::function<void(bool, QList<Pelicula>, QString)> callback,
    QObject* context) {
  QString endpoint = (idCine > 0)
                         ? QString("/api/v1/peliculas?cine_id=%1").arg(idCine)
                         : QString("/api/v1/peliculas");
  QUrl url(baseUrl + endpoint);
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [this, reply, callback]() {
    QList<Pelicula> lista;
    if (reply->error() != QNetworkReply::NoError) {
      QString errorMsg = diagnosticarErrorRed(reply, baseUrl);
      emit servidorDesconectado(errorMsg);
      callback(false, lista, errorMsg);
      reply->deleteLater();
      return;
    }

    QByteArray body = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject obj = doc.object();
    QJsonArray arr = obj["peliculas"].toArray();

    for (const QJsonValue& val : arr) {
      QJsonObject pObj = val.toObject();
      Pelicula p(pObj["id"].toInt(), pObj["titulo"].toString().toStdString(),
                 stringToGenero(pObj["genero"].toString().toStdString()),
                 pObj["duracion"].toInt());
      lista.append(p);
    }
    callback(true, lista, "");
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::obtenerPeliculas(
    int idCine, std::function<void(bool, QList<Pelicula>)> callback,
    QObject* context) {
  obtenerPeliculas(idCine, [callback](bool ok, QList<Pelicula> peliculas, const QString&) {
    callback(ok, std::move(peliculas));
  }, context);
}

void ApiClient::obtenerPeliculas(
    std::function<void(bool, QList<Pelicula>)> callback, QObject* context) {
  obtenerPeliculas(0, callback, context);
}

void ApiClient::obtenerSesiones(
    int idCine, std::function<void(bool, QList<Sesion>, QString)> callback,
    QObject* context) {
  QUrl url(baseUrl + QString("/api/v1/sesiones?cine_id=%1").arg(idCine));
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [this, reply, callback]() {
    QList<Sesion> listaSesiones;
    if (reply->error() != QNetworkReply::NoError) {
      QString errorMsg = diagnosticarErrorRed(reply, baseUrl);
      emit servidorDesconectado(errorMsg);
      callback(false, listaSesiones, errorMsg);
      reply->deleteLater();
      return;
    }

    QByteArray body = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject obj = doc.object();
    QJsonArray arr = obj["sesiones"].toArray();

    for (const QJsonValue& val : arr) {
      QJsonObject sObj = val.toObject();
      int pelId = sObj["pelicula_id"].toInt();
      std::string titulo = sObj["pelicula_titulo"].toString().toStdString();
      Genero genero = stringToGenero(sObj["pelicula_genero"].toString().toStdString());
      int duracion = sObj["pelicula_duracion"].toInt();

      Pelicula p(pelId, titulo, genero, duracion);
      Sesion s(sObj["id"].toInt(), p, sObj["sala_id"].toInt(),
               static_cast<std::time_t>(sObj["fecha_hora"].toVariant().toLongLong()));
      listaSesiones.append(s);
    }

    callback(true, listaSesiones, "");
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::obtenerSesiones(
    int idCine, std::function<void(bool, QList<Sesion>)> callback,
    QObject* context) {
  obtenerSesiones(idCine, [callback](bool ok, QList<Sesion> sesiones, const QString&) {
    callback(ok, std::move(sesiones));
  }, context);
}

void ApiClient::obtenerReservasDeSesion(
    int idSesion, std::function<void(bool, QList<Reserva>, QString)> callback,
    QObject* context) {
  QUrl url(baseUrl + QString("/api/v1/sesiones/%1/reservas").arg(idSesion));
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [this, reply, callback]() {
    QList<Reserva> listaReservas;
    if (reply->error() != QNetworkReply::NoError) {
      QString errorMsg = diagnosticarErrorRed(reply, baseUrl);
      emit servidorDesconectado(errorMsg);
      callback(false, listaReservas, errorMsg);
      reply->deleteLater();
      return;
    }

    QByteArray body = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject obj = doc.object();
    QJsonArray arr = obj["reservas"].toArray();

    for (const QJsonValue& val : arr) {
      QJsonObject rObj = val.toObject();
      Reserva r(rObj["id"].toInt(), rObj["id_sesion"].toInt(),
                rObj["fila"].toInt(), rObj["columna"].toInt(),
                rObj["estado"].toString().toStdString(), 0,
                rObj["tipo"].toString().toStdString(),
                static_cast<float>(rObj["precio"].toDouble()));
      listaReservas.append(r);
    }

    callback(true, listaReservas, "");
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::obtenerReservasDeSesion(
    int idSesion, std::function<void(bool, QList<Reserva>)> callback,
    QObject* context) {
  obtenerReservasDeSesion(idSesion, [callback](bool ok, QList<Reserva> reservas, const QString&) {
    callback(ok, std::move(reservas));
  }, context);
}

void ApiClient::obtenerSala(int idSala,
                            std::function<void(bool, Sala, QString)> callback,
                            QObject* context) {
  QUrl url(baseUrl + QString("/api/v1/salas/%1").arg(idSala));
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [this, reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      QString errorMsg = diagnosticarErrorRed(reply, baseUrl);
      emit servidorDesconectado(errorMsg);
      callback(false, Sala(), errorMsg);
      reply->deleteLater();
      return;
    }

    QByteArray body = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject obj = doc.object();

    Sala s(obj["id"].toInt(), obj["id_cine"].toInt(),
           obj["numero_sala"].toInt(), obj["filas"].toInt(),
           obj["columnas"].toInt());

    callback(true, s, "");
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::obtenerSala(int idSala,
                            std::function<void(bool, Sala)> callback,
                            QObject* context) {
  obtenerSala(idSala, [callback](bool ok, Sala sala, const QString&) {
    callback(ok, std::move(sala));
  }, context);
}

void ApiClient::autenticar(const QString& dni, const QString& password,
                           std::function<void(bool, Usuario, QString)> callback,
                           QObject* context) {
  QUrl url(baseUrl + "/api/v1/auth/login");
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject payload;
  payload["dni"] = dni;
  payload["password"] = password;

  QByteArray data = QJsonDocument(payload).toJson();
  QNetworkReply* reply = manager->post(req, data);

  auto onFinished = [this, reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      QString errorMsg;
      if (reply->error() == QNetworkReply::AuthenticationRequiredError ||
          reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
        errorMsg = "DNI o contraseña incorrectos.";
      } else {
        errorMsg = diagnosticarErrorRed(reply, baseUrl);
        emit servidorDesconectado(errorMsg);
      }

      Usuario uInvalido;
      callback(false, uInvalido, errorMsg);
      reply->deleteLater();
      return;
    }

    QByteArray body = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject obj = doc.object();

    Usuario u(obj["dni"].toString().toStdString(),
              obj["nombre"].toString().toStdString(),
              obj["apellidos"].toString().toStdString(),
              obj["email"].toString().toStdString(), "",
              obj["rol"].toString().toStdString());
    callback(true, u, "");
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::autenticar(const QString& dni, const QString& password,
                           std::function<void(bool, Usuario)> callback,
                           QObject* context) {
  autenticar(dni, password, [callback](bool ok, Usuario u, const QString&) {
    callback(ok, std::move(u));
  }, context);
}

void ApiClient::registrar(const Usuario& usuario, const QString& password,
                          std::function<void(bool, QString)> callback,
                          QObject* context) {
  QUrl url(baseUrl + "/api/v1/auth/register");
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject payload;
  payload["dni"] = QString::fromStdString(usuario.getDni());
  payload["nombre"] = QString::fromStdString(usuario.getNombre());
  payload["apellidos"] = QString::fromStdString(usuario.getApellidos());
  payload["email"] = QString::fromStdString(usuario.getEmail());
  payload["password"] = password;

  QByteArray data = QJsonDocument(payload).toJson();
  QNetworkReply* reply = manager->post(req, data);

  auto onFinished = [this, reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      QByteArray body = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(body);
      QString msg;
      if (doc.isObject() && doc.object().contains("error")) {
        msg = doc.object().value("error").toString();
      } else {
        msg = diagnosticarErrorRed(reply, baseUrl);
        emit servidorDesconectado(msg);
      }
      callback(false, msg);
    } else {
      callback(true, "Usuario registrado correctamente.");
    }
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::crearReservas(int idSesion, const QList<Reserva>& reservas,
                              std::function<void(bool, QString)> callback,
                              QObject* context) {
  QUrl url(baseUrl + "/api/v1/reservas");
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject payload;
  payload["sesion_id"] = idSesion;

  QJsonArray resArray;
  for (const auto& r : reservas) {
    QJsonObject rObj;
    rObj["fila"] = r.getFila();
    rObj["columna"] = r.getColumna();
    rObj["tipo"] = QString::fromStdString(r.getTipo());
    rObj["precio"] = r.getPrecio();
    resArray.append(rObj);
  }
  payload["reservas"] = resArray;

  QByteArray data = QJsonDocument(payload).toJson();
  QNetworkReply* reply = manager->post(req, data);

  auto onFinished = [this, reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      QByteArray body = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(body);
      QString msg;
      if (doc.isObject() && doc.object().contains("error")) {
        msg = doc.object().value("error").toString();
      } else {
        msg = diagnosticarErrorRed(reply, baseUrl);
        emit servidorDesconectado(msg);
      }
      callback(false, msg);
    } else {
      callback(true, "Reserva realizada con éxito.");
    }
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}
