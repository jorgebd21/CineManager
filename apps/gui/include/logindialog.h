#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "apiclient.h"
#include "db/datamanager.hpp"
#include "models/usuario.hpp"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
  Q_OBJECT

 public:
  explicit LoginDialog(DataManager& db, ApiClient* api = nullptr, QWidget* parent = nullptr);
  ~LoginDialog();

  Usuario getUsuarioObtenido() const { return usuarioObtenido; }
  bool estaLogueado() const { return usuarioObtenido.esValido(); }

 private slots:
  void alPulsarIniciarSesion();
  void alPulsarRegistrar();
  void alPulsarInvitado();

 private:
  Ui::LoginDialog* ui;
  DataManager& db;
  ApiClient* api;
  Usuario usuarioObtenido;
};

#endif  // LOGINDIALOG_H
