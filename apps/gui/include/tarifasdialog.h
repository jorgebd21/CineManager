#ifndef TARIFASDIALOG_H
#define TARIFASDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace Ui {
class TarifasDialog;
}

struct TarifaAsiento {
  int fila;
  int columna;
  std::string tipo;
  float precio;
};

class TarifasDialog : public QDialog {
  Q_OBJECT

 public:
  explicit TarifasDialog(const std::set<std::pair<int, int>>& butacas,
                         QWidget* parent = nullptr);
  ~TarifasDialog();

  std::vector<TarifaAsiento> getTarifasSeleccionadas() const;
  float getPrecioTotal() const;

 private slots:
  void recalcularPrecioTotal();

 private:
  Ui::TarifasDialog* ui;
  std::vector<std::pair<std::pair<int, int>, QComboBox*>> combosButacas;
  float precioTotalCalculado;
};

#endif  // TARIFASDIALOG_H
