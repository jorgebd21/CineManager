#include "apiclient.h"

ApiClient::ApiClient(QObject* parent, const QString& baseUrl)
    : QObject(parent),
      manager(new QNetworkAccessManager(this)),
      baseUrl(baseUrl) {}

void ApiClient::setBaseUrl(const QString& url) { baseUrl = url; }
QString ApiClient::getBaseUrl() const { return baseUrl; }

void ApiClient::healthCheck(std::function<void(bool)> callback,
                            QObject* context) {
  QUrl url(baseUrl + "/api/v1/health");
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [reply, callback]() {
    bool ok = (reply->error() == QNetworkReply::NoError);
    callback(ok);
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
  QUrl url(baseUrl + "/api/v1/cines");
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [reply, callback]() {
    QList<Cine> lista;
    if (reply->error() != QNetworkReply::NoError) {
      callback(false, lista);
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
    callback(true, lista);
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
  QString endpoint = (idCine > 0)
                         ? QString("/api/v1/peliculas?cine_id=%1").arg(idCine)
                         : QString("/api/v1/peliculas");
  QUrl url(baseUrl + endpoint);
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [reply, callback]() {
    QList<Pelicula> lista;
    if (reply->error() != QNetworkReply::NoError) {
      callback(false, lista);
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
    callback(true, lista);
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
}

void ApiClient::obtenerPeliculas(
    std::function<void(bool, QList<Pelicula>)> callback, QObject* context) {
  obtenerPeliculas(0, callback, context);
}

void ApiClient::obtenerSesiones(
    int idCine, std::function<void(bool, QList<Sesion>)> callback,
    QObject* context) {
  QUrl url(baseUrl + QString("/api/v1/sesiones?cine_id=%1").arg(idCine));
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [reply, callback]() {
    QList<Sesion> listaSesiones;
    if (reply->error() != QNetworkReply::NoError) {
      callback(false, listaSesiones);
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

    callback(true, listaSesiones);
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
  QUrl url(baseUrl + QString("/api/v1/sesiones/%1/reservas").arg(idSesion));
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [reply, callback]() {
    QList<Reserva> listaReservas;
    if (reply->error() != QNetworkReply::NoError) {
      callback(false, listaReservas);
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

    callback(true, listaReservas);
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
  QUrl url(baseUrl + QString("/api/v1/salas/%1").arg(idSala));
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  auto onFinished = [reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      callback(false, Sala());
      reply->deleteLater();
      return;
    }

    QByteArray body = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject obj = doc.object();

    Sala s(obj["id"].toInt(), obj["id_cine"].toInt(),
           obj["numero_sala"].toInt(), obj["filas"].toInt(),
           obj["columnas"].toInt());

    callback(true, s);
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
  QUrl url(baseUrl + "/api/v1/auth/login");
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject payload;
  payload["dni"] = dni;
  payload["password"] = password;

  QByteArray data = QJsonDocument(payload).toJson();
  QNetworkReply* reply = manager->post(req, data);

  auto onFinished = [reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      Usuario uInvalido;
      callback(false, uInvalido);
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
    callback(true, u);
    reply->deleteLater();
  };

  if (context != nullptr) {
    connect(reply, &QNetworkReply::finished, context, onFinished);
  } else {
    connect(reply, &QNetworkReply::finished, this, onFinished);
  }
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

  auto onFinished = [reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      QByteArray body = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(body);
      QString msg = doc.object().value("error").toString("Error al conectar con el servidor.");
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

  auto onFinished = [reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      QByteArray body = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(body);
      QString msg = doc.object().value("error").toString("Error al procesar la reserva.");
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
