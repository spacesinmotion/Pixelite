#include "Pixelite.h"

#include "NewImageDialog.h"
#include "ui_Pixelite.h"

#include <QCloseEvent>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>

Pixelite::Pixelite(QWidget *parent) : QMainWindow{parent}, ui{std::make_unique<Ui::Pixelite>()}
{
  ui->setupUi(this);

  background(ui->toolBar->widgetForAction(ui->acColor), ui->drawPane->currentColor());

  connect(ui->drawPane, &DrawPane::undoAvailable, ui->actionUndo, &QAction::setEnabled);
  connect(ui->drawPane, &DrawPane::redoAvailable, ui->actionRedo, &QAction::setEnabled);
  connect(ui->actionUndo, &QAction::triggered, ui->drawPane, &DrawPane::undo);
  connect(ui->actionRedo, &QAction::triggered, ui->drawPane, &DrawPane::redo);

  loadSettings();

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
  newImageD->onAppy([this, newImageD](const auto &s) {
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
  addRecent(p);
  return true;
}

void Pixelite::on_actionQuit_triggered()
{
  close();
}

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

void Pixelite::loadSettings()
{
  QSettings s;
  restoreGeometry(s.value("Main/Geometry").toByteArray());
  restoreState(s.value("Main/State").toByteArray());

  auto recent = s.value("Main/RecentFiles").toStringList();
  if (!recent.isEmpty())
    _path = recent.front();
}

void Pixelite::saveSettings() const
{
  QSettings s;
  s.setValue("Main/Geometry", saveGeometry());
  s.setValue("Main/State", saveState());
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
