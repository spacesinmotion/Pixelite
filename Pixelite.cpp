#include "Pixelite.h"

#include "ui_Pixelite.h"

#include <QColorDialog>
#include <QShortcut>

Pixelite::Pixelite(QWidget *parent) : QMainWindow{parent}, ui{std::make_unique<Ui::Pixelite>()}
{
  ui->setupUi(this);

  background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());

  connect(ui->drawPane, &DrawPane::undoAvailable, ui->actionUndo, &QAction::setEnabled);
  connect(ui->drawPane, &DrawPane::redoAvailable, ui->actionRedo, &QAction::setEnabled);
  connect(ui->actionUndo, &QAction::triggered, ui->drawPane, &DrawPane::undo);
  connect(ui->actionRedo, &QAction::triggered, ui->drawPane, &DrawPane::redo);

  ui->drawPane->newImage();
}

Pixelite::~Pixelite() = default;

void Pixelite::on_acColor_triggered()
{
  const auto c = QColorDialog::getColor(ui->drawPane->currentColor(), this);
  if (c.isValid())
  {
    ui->drawPane->setCurrentColor(c);
    background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());
  }
}

void Pixelite::on_acSelectColor_triggered(bool on)
{
  if (!on)
    return;

  ui->drawPane->pickColorMode([this] {
    ui->acSelectColor->setChecked(false);
    background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());
  });
}

void Pixelite::background(QWidget *w, const QColor &c)
{
  w->setStyleSheet(QString("background-color: %1").arg(c.name()));
}
