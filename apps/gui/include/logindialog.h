#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "db/datamanager.hpp"
#include "models/usuario.hpp"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
  Q_OBJECT

 public:
  explicit LoginDialog(DataManager& db, QWidget* parent = nullptr);
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
  Usuario usuarioObtenido;
};

#endif  // LOGINDIALOG_H
