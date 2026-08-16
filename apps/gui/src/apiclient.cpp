#include "apiclient.h"

ApiClient::ApiClient(QObject* parent, const QString& baseUrl)
    : QObject(parent), manager(new QNetworkAccessManager(this)), baseUrl(baseUrl) {}

void ApiClient::setBaseUrl(const QString& url) { baseUrl = url; }
QString ApiClient::getBaseUrl() const { return baseUrl; }

void ApiClient::healthCheck(std::function<void(bool)> callback) {
  QUrl url(baseUrl + "/api/v1/health");
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
    bool ok = (reply->error() == QNetworkReply::NoError);
    callback(ok);
    reply->deleteLater();
  });
}

void ApiClient::obtenerCines(std::function<void(bool, QList<Cine>)> callback) {
  QUrl url(baseUrl + "/api/v1/cines");
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
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
  });
}

void ApiClient::obtenerPeliculas(std::function<void(bool, QList<Pelicula>)> callback) {
  QUrl url(baseUrl + "/api/v1/peliculas");
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
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
  });
}

void ApiClient::obtenerSesiones(int idCine, std::function<void(bool, QList<Sesion>)> callback) {
  QUrl url(baseUrl + QString("/api/v1/sesiones?cine_id=%1").arg(idCine));
  QNetworkRequest req(url);

  QNetworkReply* reply = manager->get(req);
  connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
    QList<Sesion> lista;
    if (reply->error() != QNetworkReply::NoError) {
      callback(false, lista);
      reply->deleteLater();
      return;
    }

    QByteArray body = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(body);
    QJsonObject obj = doc.object();
    QJsonArray arr = obj["sesiones"].toArray();

    // Requerimos las películas para construir las sesiones
    obtenerPeliculas([reply, callback, arr](bool okPel, QList<Pelicula> peliculas) {
      QList<Sesion> listaSesiones;
      if (!okPel) {
        callback(false, listaSesiones);
        reply->deleteLater();
        return;
      }

      for (const QJsonValue& val : arr) {
        QJsonObject sObj = val.toObject();
        int pelId = sObj["pelicula_id"].toInt();

        Pelicula pelEncontrada(-1, "", Genero::DRAMA, 0);
        for (const auto& p : peliculas) {
          if (p.getId() == pelId) {
            pelEncontrada = p;
            break;
          }
        }

        Sesion s(sObj["id"].toInt(), pelEncontrada, sObj["sala_id"].toInt(),
                 (std::time_t)sObj["fecha_hora"].toVariant().toLongLong());
        listaSesiones.append(s);
      }
      callback(true, listaSesiones);
      reply->deleteLater();
    });
  });
}

void ApiClient::autenticar(const QString& dni, const QString& password,
                           std::function<void(bool, Usuario)> callback) {
  QUrl url(baseUrl + "/api/v1/auth/login");
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject payload;
  payload["dni"] = dni;
  payload["password"] = password;

  QByteArray data = QJsonDocument(payload).toJson();
  QNetworkReply* reply = manager->post(req, data);

  connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
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
  });
}

void ApiClient::registrar(const Usuario& usuario, const QString& password,
                          std::function<void(bool, QString)> callback) {
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

  connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      QByteArray body = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(body);
      QString msg = doc.object().value("error").toString("Error al conectar con el servidor.");
      callback(false, msg);
    } else {
      callback(true, "Usuario registrado correctamente.");
    }
    reply->deleteLater();
  });
}

void ApiClient::crearReservas(int idSesion, const QList<Reserva>& reservas,
                              std::function<void(bool, QString)> callback) {
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

  connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
    if (reply->error() != QNetworkReply::NoError) {
      QByteArray body = reply->readAll();
      QJsonDocument doc = QJsonDocument::fromJson(body);
      QString msg = doc.object().value("error").toString("Error al procesar la reserva.");
      callback(false, msg);
    } else {
      callback(true, "Reserva realizada con éxito.");
    }
    reply->deleteLater();
  });
}
