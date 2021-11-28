#pragma once

#include <QWidget>

class ColorSelector : public QWidget
{
  QColor _color;
  QRect _hueRect;
  QRect _satValueRect;
  bool _moveHue{false};
  bool _moveValSat{false};

public:
  ColorSelector(const QColor &c, QWidget *parent);

  static QColor getColor(const QColor &c, QWidget *parent);

private:
  void paintEvent(QPaintEvent *pe) final;

  void mouse(const QPoint &mp);
  void mousePressEvent(QMouseEvent *me) final;
  void mouseMoveEvent(QMouseEvent *me) final;
};
