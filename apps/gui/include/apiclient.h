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
#include <QPointer>
#include <QString>
#include <QUrl>
#include <functional>

#include "models/cine.hpp"
#include "models/pelicula.hpp"
#include "models/reserva.hpp"
#include "models/sala.hpp"
#include "models/sesion.hpp"
#include "models/usuario.hpp"

class ApiClient : public QObject {
  Q_OBJECT

 private:
  QNetworkAccessManager* manager;
  QString baseUrl;

 public:
  explicit ApiClient(QObject* parent = nullptr,
                     const QString& baseUrl = "http://localhost:8080");

  void setBaseUrl(const QString& url);
  [[nodiscard]] QString getBaseUrl() const;

  // Endpoints REST API Asíncronos con gestión segura de ciclo de vida
  void healthCheck(std::function<void(bool)> callback,
                   QObject* context = nullptr);
  void obtenerCines(std::function<void(bool, QList<Cine>)> callback,
                    QObject* context = nullptr);
  void obtenerPeliculas(int idCine,
                        std::function<void(bool, QList<Pelicula>)> callback,
                        QObject* context = nullptr);
  void obtenerPeliculas(std::function<void(bool, QList<Pelicula>)> callback,
                        QObject* context = nullptr);
  void obtenerSesiones(int idCine,
                       std::function<void(bool, QList<Sesion>)> callback,
                       QObject* context = nullptr);
  void obtenerReservasDeSesion(
      int idSesion, std::function<void(bool, QList<Reserva>)> callback,
      QObject* context = nullptr);
  void obtenerSala(int idSala, std::function<void(bool, Sala)> callback,
                   QObject* context = nullptr);
  void autenticar(const QString& dni, const QString& password,
                  std::function<void(bool, Usuario)> callback,
                  QObject* context = nullptr);
  void registrar(const Usuario& usuario, const QString& password,
                 std::function<void(bool, QString)> callback,
                 QObject* context = nullptr);
  void crearReservas(int idSesion, const QList<Reserva>& reservas,
                     std::function<void(bool, QString)> callback,
                     QObject* context = nullptr);
};

#endif  // API_CLIENT_H
