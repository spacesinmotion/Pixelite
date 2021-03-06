#include "Pixelite.h"

#include "ColorPalette.h"
#include "ColorSelector.h"
#include "NewImageDialog.h"
#include "ui_Pixelite.h"

#include <QActionGroup>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>

Pixelite::Pixelite(QWidget *parent) : QMainWindow{parent}, ui{std::make_unique<Ui::Pixelite>()}
{
  ui->setupUi(this);

  auto *ag = new QActionGroup(this);
  ag->addAction(ui->actionPen);
  ag->addAction(ui->acSelectColor);
  ag->addAction(ui->actionFill);
  ui->actionPen->setChecked(true);

  _colorPalette = new ColorPalette{[this] { return ui->drawPane->get_color_table(); }, this};
  connect(_colorPalette, &ColorPalette::colorClicked, this, [this](int index, const auto &mb) {
    ui->drawPane->setCurrentColorIndex(index);
    if (mb == Qt::RightButton)
      replaceCurrentColor();
    else
      background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());
  });
  ui->toolBar->addWidget(_colorPalette);

  connect(ui->drawPane, &DrawPane::undoAvailable, ui->actionUndo, &QAction::setEnabled);
  connect(ui->drawPane, &DrawPane::redoAvailable, ui->actionRedo, &QAction::setEnabled);
  connect(ui->actionUndo, &QAction::triggered, ui->drawPane, &DrawPane::undo);
  connect(ui->actionRedo, &QAction::triggered, ui->drawPane, &DrawPane::redo);
  connect(ui->drawPane, &DrawPane::imageChanged, this, [this] {
    updateWindowTitle();
    background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());
  });

  loadSettings();
  background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());

  if (_path.isEmpty())
    ui->drawPane->newImage(QSize(16, 16));
  else
    openFile(_path);
}

Pixelite::~Pixelite() = default;

void Pixelite::on_actionNew_triggered()
{
  if (!check_saved())
    return;

  ui->drawPane->hide();

  auto *newImageD = new NewImageDialog(ui->centralwidget);
  ui->centralwidget->layout()->addWidget(newImageD);
  newImageD->onCancel([this, newImageD] {
    ui->drawPane->show();
    newImageD->deleteLater();
  });
  newImageD->onAppy([this, newImageD](const auto &s) {
    _path.clear();
    ui->drawPane->newImage(s);
    ui->drawPane->show();
    newImageD->deleteLater();
  });
}

void Pixelite::on_actionOpen_triggered()
{
  if (!check_saved())
    return;

  openFile(QFileDialog::getOpenFileName(this, qApp->applicationDisplayName(), _path,
                                        tr("Images (*.png *.xpm *.jpg)")));
}

void Pixelite::on_menuRecent_aboutToShow()
{
  auto actions = ui->menuRecent->actions();
  actions.removeAll(ui->actionclear_recent);
  qDeleteAll(actions);

  const auto recent = QSettings().value("Main/RecentFiles").toStringList();
  for (const auto &p : recent)
    ui->menuRecent->addAction(QFileInfo(p).fileName(), this, [this, p] { openFile(p); });
  ui->menuRecent->addSeparator();
  ui->menuRecent->addAction(ui->actionclear_recent);
}

void Pixelite::on_actionclear_recent_triggered()
{
  QSettings s;
  s.remove("Main/RecentFiles");
}

bool Pixelite::on_actionSave_triggered()
{
  if (_path.isEmpty())
    return on_actionSave_as_triggered();

  ui->drawPane->currentImage().save(_path);
  ui->drawPane->mark_saved();
  updateWindowTitle();
  return true;
}

bool Pixelite::on_actionSave_as_triggered()
{
  const auto p = QFileDialog::getSaveFileName(this, qApp->applicationDisplayName(), _path,
                                              tr("Images (*.png *.xpm *.jpg)"));

  if (p.isEmpty())
    return false;

  _path = p;
  ui->drawPane->currentImage().save(_path);
  ui->drawPane->mark_saved();
  updateWindowTitle();
  addRecent(p);
  return true;
}

void Pixelite::on_actionQuit_triggered()
{
  close();
}

void Pixelite::on_acColor_triggered()
{
  const auto c = ColorSelector::getColor(ui->drawPane->currentColor(), this);
  if (c.isValid())
  {
    ui->drawPane->setCurrentColor(c);
    background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());
  }
}

void Pixelite::on_actionPen_triggered(bool on)
{
  if (!on)
    return;

  ui->drawPane->drawMode();
}

void Pixelite::on_acSelectColor_triggered(bool on)
{
  if (!on)
    return;

  ui->drawPane->pickColorMode([this] {
    ui->actionPen->trigger();
    background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());
  });
}

void Pixelite::on_actionFill_triggered(bool on)
{
  if (!on)
    return;

  ui->drawPane->fillColorMode([this] { ui->actionPen->trigger(); });
}

void Pixelite::on_actionReduce_Palette_triggered()
{
  ui->drawPane->reducePalette();
  _colorPalette->update();
}

void Pixelite::background(QWidget *w, const QColor &c)
{
  w->setStyleSheet(QString("background-color: %1").arg(c.name()));
}

void Pixelite::replaceCurrentColor()
{
  const auto c = ColorSelector::getColor(ui->drawPane->currentColor(), this);
  if (c.isValid())
    ui->drawPane->replaceCurrentColor(c);

  background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());
}

void Pixelite::loadSettings()
{
  QSettings s;
  restoreGeometry(s.value("Main/Geometry").toByteArray());
  restoreState(s.value("Main/State").toByteArray());

  // ui->drawPane->setCurrentColor(QColor(s.value("Main/Color", "#000000").toString()));

  const auto recent = s.value("Main/RecentFiles").toStringList();
  if (!recent.isEmpty())
    _path = recent.front();
}

void Pixelite::saveSettings() const
{
  QSettings s;
  s.setValue("Main/Geometry", saveGeometry());
  s.setValue("Main/State", saveState());
  // s.setValue("Main/Color", ui->drawPane->currentColor().name(QColor::HexArgb));
}

void Pixelite::updateWindowTitle()
{
  setWindowTitle(
      qApp->applicationDisplayName() +
      QString(" (%1)").arg(_path.isEmpty() ? "<new_image>" : QFileInfo(_path).fileName()) +
      (ui->drawPane->has_changed() ? "*" : ""));
}

void Pixelite::closeEvent(QCloseEvent *ce)
{
  if (!check_saved())
    ce->ignore();
  else
  {
    saveSettings();
    QMainWindow::closeEvent(ce);
  }
}

bool Pixelite::check_saved()
{
  if (!ui->drawPane->has_changed())
    return true;

  const auto a = QMessageBox::question(this, qApp->applicationDisplayName(),
                                       tr("You have unsaved changes! Save now?"), QMessageBox::Yes,
                                       QMessageBox::No, QMessageBox::Cancel);
  if (a == QMessageBox::Cancel)
    return false;
  else if (a == QMessageBox::Yes)
    return on_actionSave_triggered();

  return true;
}

void Pixelite::openFile(const QString &p)
{
  if (p.isEmpty())
    return;

  _path = p;
  ui->drawPane->setCurrentImage(QImage(_path));

  addRecent(p);
}

void Pixelite::addRecent(const QString &p)
{
  QSettings s;
  auto recent = s.value("Main/RecentFiles").toStringList();
  recent.removeAll(_path);
  recent.prepend(_path);
  while (recent.size() > 15)
    recent.pop_back();
  s.setValue("Main/RecentFiles", recent);
}
