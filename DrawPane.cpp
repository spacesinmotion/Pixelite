#include "DrawPane.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

DrawPane::DrawPane(QWidget *parent)
    : QWidget{parent}
{
  setMouseTracking(true);

  _img = QImage(16, 16, QImage::Format_ARGB32);
  for (int i = 0; i < 16; ++i)
  {
    for (int j = 0; j < 16; ++j)
    {
      if (i % 2 == 0 && j % 2 == 0)
        _img.setPixel(i, j, QColor(Qt::red).rgba());
      else
        _img.setPixel(i, j, QColor(Qt::transparent).rgba());
    }
  }
  _back_img = QImage(_img.width() * 2, _img.height() * 2, QImage::Format_ARGB32);
  for (int i = 0; i < _back_img.width(); ++i)
  {
    for (int j = 0; j < _back_img.height(); ++j)
    {
      if (((i % 2 == 0) && (j % 2 != 0)) || (i % 2 != 0) && (j % 2 == 0))
        _back_img.setPixel(i, j, QColor(240, 240, 240).rgba());
      else
        _back_img.setPixel(i, j, QColor(220, 220, 220).rgba());
    }
  }
}

void DrawPane::draw(const QPoint &p, const QColor &c)
{
  _img.setPixel(p, c.rgba());
}

void DrawPane::leftAction()
{
  if (_img.rect().contains(_pixel))
    draw(_pixel, _currentColor);
}

void DrawPane::rightAction()
{
  if (_img.rect().contains(_pixel))
    draw(_pixel, Qt::transparent);
}

void DrawPane::paintEvent(QPaintEvent *pe)
{
  if (_transform.isIdentity())
  {
    const auto s = qreal(std::min(width(), height())) / 1.1 / qreal(_img.height());
    _transform.scale(s, s);
    _transform.translate(width() / s / 2 - _img.width() / 2.0, height() / s / 2.0 - _img.height() / 2.0);
  }

  QPainter p(this);

  auto x = _transform;
  x.scale(0.5, 0.5);
  p.setTransform(x);
  p.drawImage(0, 0, _back_img);

  p.setTransform(_transform);
  p.drawImage(0, 0, _img);

  p.setPen(QPen(Qt::black, 0.0));
  p.drawRect(0, 0, _img.width(), _img.height());
  p.setPen(QPen(Qt::red, 0.0));
  p.drawRect(QRect(_pixel, QSize(1, 1)));

  return QWidget::paintEvent(pe);
}

void DrawPane::mousePressEvent(QMouseEvent *me)
{
  _mouse_start = me->pos();
  QWidget::mousePressEvent(me);

  if (me->buttons() == Qt::LeftButton)
    leftAction();
  else if (me->buttons() == Qt::RightButton)
    rightAction();
  else
    return;
  update();
}

void DrawPane::mouseMoveEvent(QMouseEvent *me)
{
  auto it = _transform.inverted();
  bool needUpdate = false;

  const auto _pf = it.map(QPointF(me->pos()));
  const auto _p = QPoint(std::floor(_pf.x()), std::floor(_pf.y()));
  if (_p != _pixel)
  {
    needUpdate = true;
    _pixel = _p;

    if (me->buttons() == Qt::LeftButton)
      leftAction();
    if (me->buttons() == Qt::RightButton)
      rightAction();
  }

  if (me->buttons() == Qt::MidButton)
  {
    auto m = it.map(QPointF(me->pos())) - it.map(QPointF(_mouse_start));
    _transform.translate(m.x(), m.y());
    needUpdate = true;
  }

  _mouse_start = me->pos();

  if (needUpdate)
    update();

  QWidget::mouseMoveEvent(me);
}

void DrawPane::wheelEvent(QWheelEvent *we)
{
  const auto mp = _transform.inverted().map(QPointF(we->position()));

  const auto d = we->modifiers() == Qt::ControlModifier ? 2400.0 : 800.0;
  const auto s = 1.0 - qreal(we->pixelDelta().y()) / d;
  _transform.scale(s, s);
  const auto m = _transform.inverted().map(QPointF(we->position())) - mp;
  _transform.translate(m.x(), m.y());
  update();

  QWidget::wheelEvent(we);
}
