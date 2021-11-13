#pragma once

#include <QWidget>

class DrawPane : public QWidget
{
  Q_OBJECT

  QImage _img;
  QImage _back_img;
  QTransform _transform;
  QPoint _mouse_start;

  QPoint _pixel;

  QColor _currentColor;

public:
  explicit DrawPane(QWidget *parent = nullptr);

private:
  void draw(const QPoint &p, const QColor &c);
  void leftAction();
  void rightAction();

  void paintEvent(QPaintEvent *pe);
  void mousePressEvent(QMouseEvent *me);
  void mouseMoveEvent(QMouseEvent *me);
  void wheelEvent(QWheelEvent *we);
};
