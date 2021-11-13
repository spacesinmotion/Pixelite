#pragma once

#include <QWidget>
#include <functional>

class DrawPane : public QWidget
{
  Q_OBJECT

  QImage _img;
  QImage _back_img;
  QTransform _transform;
  QPoint _mouse_start;

  QPoint _pixel;

  QColor _currentColor;

  enum Mode
  {
    Draw,
    PickColor
  };
  Mode _currentMode{Draw};

  using CB = std::function<void()>;
  CB _onFished;

public:
  explicit DrawPane(QWidget *parent = nullptr);

  const QColor &currentColor() { return _currentColor; }
  void setCurrentColor(const QColor &c)
  {
    _currentColor = c;
    update();
  }

  void drawMode() { _currentMode = Draw; }
  void pickColorMode(const CB &onFinish)
  {
    _currentMode = PickColor;
    _onFished = onFinish;
  }

private:
  void draw(const QPoint &p, const QColor &c);
  void leftAction();
  void rightAction();

  void paintEvent(QPaintEvent *pe);
  void mousePressEvent(QMouseEvent *me);
  void mouseMoveEvent(QMouseEvent *me);
  void wheelEvent(QWheelEvent *we);
};
