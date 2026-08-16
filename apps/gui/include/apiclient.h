#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QUrl>
#include <functional>

#include "models/cine.hpp"
#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include "models/sesion.hpp"
#include "models/usuario.hpp"

class ApiClient : public QObject {
  Q_OBJECT

 private:
  QNetworkAccessManager* manager;
  QString baseUrl;

 public:
  explicit ApiClient(QObject* parent = nullptr, const QString& baseUrl = "http://localhost:8080");

  void setBaseUrl(const QString& url);
  QString getBaseUrl() const;

  // Endpoints REST API
  void healthCheck(std::function<void(bool)> callback);
  void obtenerCines(std::function<void(bool, QList<Cine>)> callback);
  void obtenerPeliculas(std::function<void(bool, QList<Pelicula>)> callback);
  void obtenerSesiones(int idCine, std::function<void(bool, QList<Sesion>)> callback);
  void autenticar(const QString& dni, const QString& password, std::function<void(bool, Usuario)> callback);
  void registrar(const Usuario& usuario, const QString& password, std::function<void(bool, QString)> callback);
  void crearReservas(int idSesion, const QList<Reserva>& reservas, std::function<void(bool, QString)> callback);
};

#endif  // API_CLIENT_H
