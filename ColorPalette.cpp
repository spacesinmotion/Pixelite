#include "ColorPalette.h"

#include <QMouseEvent>
#include <QPainter>

ColorPalette::ColorPalette(const std::function<QVector<QRgb>()> &getter, QWidget *parent) :
    QWidget{parent}, _colorGetter{getter}
{
  setMinimumSize(24, 24 * 6);
}

ColorPalette::~ColorPalette()
{}

void ColorPalette::eachRectColor(const std::function<bool(const QRect &, QRgb)> &cb) const
{
  auto colors = _colorGetter();

  const auto pix = width() / 3;
  const int l = width() / pix;

  colors.removeAll(qRgba(0, 0, 0, 0));
  auto c = colors.begin();

  int line = 0;
  while (c < colors.end())
  {
    for (int i = 0; i < l; ++i)
    {
      if (c >= colors.end())
        break;

      if (!cb(QRect(i * pix, line * pix, pix, pix), *c))
        break;
      ++c;
    }
    ++line;
  }
}

void ColorPalette::paintEvent(QPaintEvent *pe)
{
  QPainter p{this};

  p.setPen(Qt::transparent);
  eachRectColor([&p](const auto &r, const auto &c) {
    p.setBrush(QColor::fromRgba(c));
    p.drawRect(r);
    return true;
  });

  p.setPen(Qt::black);
  p.setBrush(Qt::transparent);
  p.drawRect(0, 0, width() - 1, height() - 1);
}

void ColorPalette::mouseReleaseEvent(QMouseEvent *me)
{
  const auto mp = me->pos();
  eachRectColor([&](const auto &r, const auto &c) {
    if (r.contains(mp))
    {
      emit colorClicked(c);
      return false;
    }
    return true;
  });
  QWidget::mouseReleaseEvent(me);
}
