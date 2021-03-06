#pragma once

#include <QMainWindow>
#include <memory>

namespace Ui
{
class Pixelite;
}
class ColorPalette;

class Pixelite : public QMainWindow
{
  Q_OBJECT

  std::unique_ptr<Ui::Pixelite> ui;
  ColorPalette *_colorPalette{nullptr};

  QString _path;

public:
  Pixelite(QWidget *parent = nullptr);
  ~Pixelite();

private slots:
  void on_actionNew_triggered();
  void on_actionOpen_triggered();
  void on_menuRecent_aboutToShow();
  void on_actionclear_recent_triggered();
  bool on_actionSave_triggered();
  bool on_actionSave_as_triggered();
  void on_actionQuit_triggered();

  void on_acColor_triggered();
  void on_actionPen_triggered(bool);
  void on_acSelectColor_triggered(bool);
  void on_actionFill_triggered(bool);
  void on_actionReduce_Palette_triggered();

private:
  static void background(QWidget *w, const QColor &c);

  void replaceCurrentColor();

  void loadSettings();
  void saveSettings() const;

  void updateWindowTitle();

  void closeEvent(QCloseEvent *ce) final;

  bool check_saved();
  void openFile(const QString &path);
  void addRecent(const QString &path);
};
