#include "tarifasdialog.h"
#include "ui_tarifasdialog.h"

TarifasDialog::TarifasDialog(const std::set<std::pair<int, int>>& butacas,
                             QWidget* parent)
    : QDialog(parent), ui(new Ui::TarifasDialog), precioTotalCalculado(0.0f) {
  ui->setupUi(this);

  connect(ui->botonAceptar, &QPushButton::clicked, this, &QDialog::accept);
  connect(ui->botonCancelar, &QPushButton::clicked, this, &QDialog::reject);

  for (const auto& butaca : butacas) {
    int fila = butaca.first;
    int columna = butaca.second;

    QWidget* filaWidget = new QWidget(ui->scrollAreaWidgetContents);
    QHBoxLayout* filaLayout = new QHBoxLayout(filaWidget);
    filaLayout->setContentsMargins(8, 4, 8, 4);

    QLabel* labelAsiento = new QLabel(
        QString("Fila %1 - Asiento %2:")
            .arg(fila + 1)
            .arg(columna + 1),
        filaWidget);
    labelAsiento->setStyleSheet("font-weight: bold; font-size: 13px; color: #ffffff;");

    QComboBox* comboTarifa = new QComboBox(filaWidget);
    comboTarifa->setStyleSheet(
        "QComboBox {"
        "  background-color: #282c34;"
        "  color: #ffffff;"
        "  border: 1px solid #3e4452;"
        "  border-radius: 4px;"
        "  padding: 4px 8px;"
        "  combobox-popup: 0;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #1e222b;"
        "  color: #ffffff;"
        "  selection-background-color: #00f0b5;"
        "  selection-color: #1e222b;"
        "}");

    comboTarifa->addItem("Adulto - 7.50 €");
    comboTarifa->setItemData(0, "Adulto", Qt::UserRole);
    comboTarifa->setItemData(0, 7.50f, Qt::UserRole + 1);

    comboTarifa->addItem("Niño - 5.00 €");
    comboTarifa->setItemData(1, "Niño", Qt::UserRole);
    comboTarifa->setItemData(1, 5.00f, Qt::UserRole + 1);

    comboTarifa->addItem("Jubilado - 5.50 €");
    comboTarifa->setItemData(2, "Jubilado", Qt::UserRole);
    comboTarifa->setItemData(2, 5.50f, Qt::UserRole + 1);

    comboTarifa->addItem("Estudiante - 5.50 €");
    comboTarifa->setItemData(3, "Estudiante", Qt::UserRole);
    comboTarifa->setItemData(3, 5.50f, Qt::UserRole + 1);

    filaLayout->addWidget(labelAsiento);
    filaLayout->addStretch();
    filaLayout->addWidget(comboTarifa);

    ui->layoutFilas->addWidget(filaWidget);

    combosButacas.push_back({butaca, comboTarifa});

    connect(comboTarifa, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TarifasDialog::recalcularPrecioTotal);
  }

  recalcularPrecioTotal();
}

TarifasDialog::~TarifasDialog() { delete ui; }

void TarifasDialog::recalcularPrecioTotal() {
  precioTotalCalculado = 0.0f;
  for (const auto& item : combosButacas) {
    QComboBox* combo = item.second;
    int idx = combo->currentIndex();
    float precio = combo->itemData(idx, Qt::UserRole + 1).toFloat();
    precioTotalCalculado += precio;
  }

  ui->labelPrecioTotal->setText(
      QString("Total: %1 €")
          .arg(QString::number(precioTotalCalculado, 'f', 2)));
}

std::vector<TarifaAsiento> TarifasDialog::getTarifasSeleccionadas() const {
  std::vector<TarifaAsiento> resultado;
  for (const auto& item : combosButacas) {
    int f = item.first.first;
    int c = item.first.second;
    QComboBox* combo = item.second;
    int idx = combo->currentIndex();

    std::string tipo = combo->itemData(idx, Qt::UserRole).toString().toStdString();
    float precio = combo->itemData(idx, Qt::UserRole + 1).toFloat();

    resultado.push_back({f, c, tipo, precio});
  }
  return resultado;
}

float TarifasDialog::getPrecioTotal() const {
  return precioTotalCalculado;
}
