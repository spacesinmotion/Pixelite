#include "ColorSelector.h"

#include <QDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

ColorSelector::ColorSelector(const QColor &c, QWidget *parent) : QWidget{parent}, _color{c}
{
  setMinimumSize(255 + 128 + 20, 255 + 64 + 20);
}

QColor ColorSelector::getColor(const QColor &c, QWidget *parent)
{
  QDialog d(parent);
  auto *l = new QVBoxLayout{&d};
  l->setContentsMargins(0, 0, 0, 0);

  auto *cs = new ColorSelector{c, &d};
  l->addWidget(cs);

  d.exec();
  return cs->_color;
}

void ColorSelector::paintEvent(QPaintEvent *pe)
{
  QPainter p(this);

  p.setPen(Qt::transparent);
  p.setBrush(_color);
  p.drawRect(10, 10, 128, 128);

  auto x = _color;
  _satValueRect = QRect(10 + 128, 10, 256, 256);
  for (int s = 0; s < _satValueRect.width(); ++s)
  {
    for (int v = 0; v < _satValueRect.height(); ++v)
    {
      x.setHsv(_color.hue(), s, 255 - v);
      p.setPen(x);
      p.drawPoint(_satValueRect.left() + s, _satValueRect.top() + v);
    }
  }

  p.setPen(Qt::white);
  p.setBrush(_color);
  p.drawEllipse(QPoint(_satValueRect.left() + _color.hsvSaturation(),
                       _satValueRect.bottom() - _color.value()),
                7, 7);

  _hueRect = QRect(20, 255 + 20, 360, 8);
  for (int h = 0; h < _hueRect.width(); ++h)
  {
    p.setPen(QColor::fromHsv(h, 255, 255));
    p.drawLine(_hueRect.left() + h, _hueRect.top(), _hueRect.left() + h,
               _hueRect.top() + _hueRect.height());
  }
  p.setPen(Qt::white);
  p.setBrush(QColor::fromHsv(_color.hsvHue(), 255, 255));
  p.drawEllipse(QPoint(_hueRect.left() + _color.hsvHue(), _hueRect.top() + _hueRect.height() / 2),
                7, 7);
}

void ColorSelector::mouse(const QPoint &mp)
{
  if (!_moveHue && !_moveValSat)
    return;

  const auto h = _moveHue ? mp.x() - _hueRect.left() : _color.hsvHue();
  const auto s = _moveValSat ? mp.x() - _satValueRect.left() : _color.hsvSaturation();
  const auto v = _moveValSat ? _satValueRect.bottom() - mp.y() : _color.value();
  _color.setHsv(qMax(9, qMin(359, h)), qMax(0, qMin(255, s)), qMax(0, qMin(255, v)),
                _color.alpha());
  update();
}

void ColorSelector::mousePressEvent(QMouseEvent *me)
{
  _moveHue = _hueRect.contains(me->pos());
  _moveValSat = _satValueRect.contains(me->pos());
  mouse(me->pos());

  QWidget::mousePressEvent(me);
}

void ColorSelector::mouseMoveEvent(QMouseEvent *me)
{
  mouse(me->pos());
  QWidget::mouseMoveEvent(me);
}