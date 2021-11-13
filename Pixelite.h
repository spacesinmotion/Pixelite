#pragma once

#include <memory>
#include <QMainWindow>

namespace Ui
{
  class Pixelite;
}

class Pixelite : public QMainWindow
{
  Q_OBJECT

  std::unique_ptr<Ui::Pixelite> ui;

public:
  Pixelite(QWidget *parent = nullptr);
  ~Pixelite();

private slots:
  void on_pbForegroundColor_clicked();

private:
  static void background(QWidget *w, const QColor &c);
};
