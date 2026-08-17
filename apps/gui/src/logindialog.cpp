#include "logindialog.h"

#include "ui_logindialog.h"

LoginDialog::LoginDialog(ApiClient* api, QWidget* parent)
    : QDialog(parent), ui(new Ui::LoginDialog), api(api) {
  ui->setupUi(this);

  // Paleta con placeholder de baja opacidad / alpha y texto 100% blanco al escribir
  const auto lineEdits = findChildren<QLineEdit*>();
  for (QLineEdit* le : lineEdits) {
    QPalette p = le->palette();
    p.setColor(QPalette::PlaceholderText, QColor(140, 145, 165, 120));
    p.setColor(QPalette::Text, QColor(255, 255, 255, 255));
    le->setPalette(p);

    auto actualizarTono = [le]() {
      if (le->text().isEmpty()) {
        // Sin texto: baja opacidad/alpha atenuado indicando que es un ejemplo/placeholder
        le->setStyleSheet(
            "QLineEdit {"
            "  background-color: rgba(18, 20, 30, 0.45);"
            "  border: 1px solid rgba(43, 46, 61, 0.4);"
            "  border-radius: 6px;"
            "  color: rgba(255, 255, 255, 0.5);"
            "  font-size: 13px;"
            "  padding: 8px 12px;"
            "}"
            "QLineEdit:focus {"
            "  background-color: rgba(26, 30, 44, 0.85);"
            "  border: 1px solid #61AFEF;"
            "  color: #FFFFFF;"
            "}");
      } else {
        // Con texto: sólido, 100% opaco, realce activo y texto blanco puro brillante
        le->setStyleSheet(
            "QLineEdit {"
            "  background-color: #1E2232;"
            "  border: 1px solid #61AFEF;"
            "  border-radius: 6px;"
            "  color: #FFFFFF;"
            "  font-weight: bold;"
            "  font-size: 13px;"
            "  padding: 8px 12px;"
            "}");
      }
    };

    actualizarTono();
    connect(le, &QLineEdit::textChanged, this, [actualizarTono](const QString&) {
      actualizarTono();
    });
  }

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
