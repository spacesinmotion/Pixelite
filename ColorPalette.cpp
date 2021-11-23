#include "ColorPalette.h"

#include <QPainter>

ColorPalette::ColorPalette(const std::function<QVector<QRgb>()> &getter, QWidget *parent) :
    QWidget{parent}, _colorGetter{getter}
{
  setMinimumSize(24, 24 * 6);
}

ColorPalette::~ColorPalette()
{}

void ColorPalette::paintEvent(QPaintEvent *pe)
{
  QPainter p{this};

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

      p.setPen(Qt::transparent);
      p.setBrush(QColor(*c));
      p.drawRect(i * pix, line * pix, pix, pix);
      ++c;
    }
    ++line;
  }

  p.setPen(Qt::black);
  p.setBrush(Qt::transparent);
  p.drawRect(0, 0, width() - 1, height() - 1);
}
