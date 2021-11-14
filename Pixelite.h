#pragma once

#include <QMainWindow>
#include <memory>

namespace Ui
{
class Pixelite;
}

class Pixelite : public QMainWindow
{
  Q_OBJECT

  std::unique_ptr<Ui::Pixelite> ui;

  QString _path;

public:
  Pixelite(QWidget *parent = nullptr);
  ~Pixelite();

private slots:
  void on_actionNew_triggered();
  void on_actionOpen_triggered();
  bool on_actionSave_triggered();
  bool on_actionSave_as_triggered();
  void on_actionQuit_triggered();

  void on_acColor_triggered();
  void on_acSelectColor_triggered(bool);

private:
  static void background(QWidget *w, const QColor &c);

  bool check_saved();
};
