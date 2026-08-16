#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "apiclient.h"
#include "models/usuario.hpp"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
  Q_OBJECT

 public:
  explicit LoginDialog(ApiClient* api, QWidget* parent = nullptr);
  ~LoginDialog();

  [[nodiscard]] const Usuario& getUsuarioObtenido() const noexcept {
    return usuarioObtenido;
  }
  [[nodiscard]] bool estaLogueado() const noexcept {
    return usuarioObtenido.esValido();
  }

 private slots:
  void alPulsarIniciarSesion();
  void alPulsarRegistrar();
  void alPulsarInvitado();

 private:
  Ui::LoginDialog* ui;
  ApiClient* api;
  Usuario usuarioObtenido;
};

#endif  // LOGINDIALOG_H
