#include "logindialog.h"

#include "ui_logindialog.h"

LoginDialog::LoginDialog(DataManager& db, QWidget* parent)
    : QDialog(parent), ui(new Ui::LoginDialog), db(db) {
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
  std::string dni = ui->inputDniLogin->text().trimmed().toStdString();
  std::string pass = ui->inputPasswordLogin->text().toStdString();

  if (dni.empty() || pass.empty()) {
    ui->lblErrorLogin->setText("Por favor, rellena todos los campos.");
    return;
  }

  Usuario user = db.autenticarUsuario(dni, pass);
  if (user.esValido()) {
    usuarioObtenido = user;
    accept();
  } else {
    ui->lblErrorLogin->setText("DNI o contraseña incorrectos.");
  }
}

void LoginDialog::alPulsarRegistrar() {
  ui->lblErrorRegistro->clear();
  std::string dni = ui->inputDniRegistro->text().trimmed().toStdString();
  std::string nombre = ui->inputNombreRegistro->text().trimmed().toStdString();
  std::string apellidos =
      ui->inputApellidosRegistro->text().trimmed().toStdString();
  std::string email = ui->inputEmailRegistro->text().trimmed().toStdString();
  std::string pass = ui->inputPasswordRegistro->text().toStdString();

  if (dni.empty() || nombre.empty() || apellidos.empty() || email.empty() ||
      pass.empty()) {
    ui->lblErrorRegistro->setText("Por favor, completa todos los campos.");
    return;
  }

  // Verificar si el DNI ya existe
  if (db.obtenerUsuario(dni).esValido()) {
    ui->lblErrorRegistro->setText("El DNI ya se encuentra registrado.");
    return;
  }

  Usuario nuevoUser(dni, nombre, apellidos, email, pass, "CLIENTE");
  if (db.crearUsuario(nuevoUser)) {
    usuarioObtenido = nuevoUser;
    accept();
  } else {
    ui->lblErrorRegistro->setText("Error al registrar el usuario en la BD.");
  }
}

void LoginDialog::alPulsarInvitado() {
  usuarioObtenido = Usuario();  // DNI vacío -> modo invitado
  accept();
}
