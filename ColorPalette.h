#pragma

#include <QWidget>

class ColorPalette : public QWidget
{
  Q_OBJECT

private:
  std::function<QVector<QRgb>()> _colorGetter;

public:
  ColorPalette(const std::function<QVector<QRgb>()> &getter, QWidget *parent);
  ~ColorPalette();

signals:
  void colorClicked(int index, Qt::MouseButton mb);

private:
  void eachRectColor(const std::function<bool(const QRect &, QRgb)> &cb) const;

  void paintEvent(QPaintEvent *pe) final;
  void mouseReleaseEvent(QMouseEvent *me) final;
};
