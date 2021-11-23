#pragma

#include <QWidget>

class ColorPalette : public QWidget
{
private:
  std::function<QVector<QRgb>()> _colorGetter;

public:
  ColorPalette(const std::function<QVector<QRgb>()> &getter, QWidget *parent);
  ~ColorPalette();

  void paintEvent(QPaintEvent *pe) final;
};
