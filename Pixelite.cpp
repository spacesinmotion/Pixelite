#include "Pixelite.h"
#include "ui_Pixelite.h"

#include <QColorDialog>

Pixelite::Pixelite(QWidget *parent)
    : QMainWindow{parent}, ui{std::make_unique<Ui::Pixelite>()}
{
  ui->setupUi(this);

  background(ui->pbForegroundColor, ui->drawPane->currentColor());
}

Pixelite::~Pixelite() = default;

void Pixelite::on_pbForegroundColor_clicked()
{
  const auto c = QColorDialog::getColor(ui->drawPane->currentColor(), this);
  if (c.isValid())
  {
    ui->drawPane->setCurrentColor(c);
    background(ui->pbForegroundColor, c);
  }
}

void Pixelite::on_acSelectColor_triggered(bool on)
{
  if (!on)
    return;

  ui->drawPane->pickColorMode([this]
                              {
                                ui->acSelectColor->setChecked(false);
                                background(ui->pbForegroundColor, ui->drawPane->currentColor());
                              });
}

void Pixelite::background(QWidget *w, const QColor &c)
{
  w->setStyleSheet(QString("background-color: %1").arg(c.name()));
}
