#include "logindialog.h"

#include "ui_logindialog.h"

LoginDialog::LoginDialog(ApiClient* api, QWidget* parent)
    : QDialog(parent), ui(new Ui::LoginDialog), api(api) {
  ui->setupUi(this);

  connect(ui->btnIniciarSesion, &QPushButton::clicked, this,
          &LoginDialog::alPulsarIniciarSesion);
  connect(ui->btnRegistrar, &QPushButton::clicked, this,
          &LoginDialog::alPulsarRegistrar);
  connect(ui->btnInvitado, &QPushButton::clicked, this,
          &LoginDialog::alPulsarInvitado);
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::alPulsarIniciarSesion() {
  ui->lblErrorLogin->clear();
  QString dni = ui->inputDniLogin->text().trimmed();
  QString pass = ui->inputPasswordLogin->text();

  if (dni.isEmpty() || pass.isEmpty()) {
    ui->lblErrorLogin->setText("Por favor, rellena todos los campos.");
    return;
  }

  if (api) {
    ui->lblErrorLogin->setText("Conectando con el servidor...");
    api->autenticar(
        dni, pass,
        [this](bool ok, Usuario user, QString errorMsg) {
          if (ok && user.esValido()) {
            usuarioObtenido = std::move(user);
            accept();
          } else {
            if (!errorMsg.isEmpty()) {
              ui->lblErrorLogin->setText(errorMsg);
            } else {
              ui->lblErrorLogin->setText("DNI o contraseña incorrectos.");
            }
          }
        },
        this);
  }
}

void LoginDialog::alPulsarRegistrar() {
  ui->lblErrorRegistro->clear();
  QString dni = ui->inputDniRegistro->text().trimmed();
  QString nombre = ui->inputNombreRegistro->text().trimmed();
  QString apellidos = ui->inputApellidosRegistro->text().trimmed();
  QString email = ui->inputEmailRegistro->text().trimmed();
  QString pass = ui->inputPasswordRegistro->text();

  if (dni.isEmpty() || nombre.isEmpty() || apellidos.isEmpty() ||
      email.isEmpty() || pass.isEmpty()) {
    ui->lblErrorRegistro->setText("Por favor, completa todos los campos.");
    return;
  }

  Usuario nuevoUser(dni.toStdString(), nombre.toStdString(),
                    apellidos.toStdString(), email.toStdString(),
                    pass.toStdString(), "CLIENTE");

  if (api) {
    ui->lblErrorRegistro->setText("Registrando en el servidor...");
    api->registrar(
        nuevoUser, pass,
        [this, nuevoUser](bool ok, QString msg) {
          if (ok) {
            usuarioObtenido = nuevoUser;
            accept();
          } else {
            ui->lblErrorRegistro->setText(msg);
          }
        },
        this);
  }
}

void LoginDialog::alPulsarInvitado() {
  usuarioObtenido = Usuario();  // Modo invitado
  accept();
}
