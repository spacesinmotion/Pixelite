#include "DrawPane.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QSet>
#include <QWheelEvent>
#include <cmath>

DrawPane::DrawPane(QWidget *parent) : QWidget{parent}
{
  setMouseTracking(true);
}

void DrawPane::setCurrentColorIndex(int i)
{
  _currentColorIndex = i;
  action_done();
  update();
}

void DrawPane::setCurrentColor(const QColor &c)
{
  auto i = _img.colorTable().indexOf(c.rgba());
  if (i < 0)
  {
    start_action();
    i = _img.colorCount();
    _img.setColorCount(i + 1);
    _img.setColor(i, c.rgba());
  }
  setCurrentColorIndex(i);
}

void DrawPane::replaceCurrentColor(const QColor &c)
{
  _img.setColor(_currentColorIndex, c.rgba());
  update();
}

QVector<QRgb> DrawPane::calc_color_table() const
{
  QSet<QRgb> unique{qRgba(0, 0, 0, 0)};
  for (int i = 0; i < _img.width(); ++i)
    for (int j = 0; j < _img.height(); ++j)
      unique.insert(_img.pixelColor(i, j).rgba());

  auto vec = unique.values().toVector();
  std::sort(vec.begin(), vec.end());
  return vec;
}

QVector<QRgb> DrawPane::get_color_table() const
{
  return _img.colorTable();
}

void DrawPane::reducePalette()
{
  start_action();
  auto x = calc_color_table();
  _currentColorIndex = qMax(1, x.indexOf(currentColor().rgba()));
  QHash<QRgb, int> h;
  for (int i = 0; i < x.size(); ++i)
    h[x[i]] = i;
  QImage im(_img.size(), QImage::Format_Indexed8);
  im.setColorTable(x);
  for (int i = 0; i < im.width(); ++i)
    for (int j = 0; j < im.width(); ++j)
      im.setPixel(i, j, h[_img.pixel(i, j)]);
  _img = im;
  action_done();
}

void DrawPane::setCurrentImage(const QImage &img)
{
  _undoStack.clear();
  _redoStack.clear();
  emit undoAvailable(false);
  emit redoAvailable(false);
  mark_saved();
  _img = img;
  if (_img.format() != QImage::Format_Indexed8)
    reducePalette();

  _currentColorIndex = std::min(1, _img.colorCount());
  action_done();

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

void DrawPane::newImage(const QSize &s)
{
  QImage i(s, QImage::Format_Indexed8);
  i.fill(Qt::transparent);
  setCurrentImage(i);
}

void DrawPane::undo()
{
  if (_undoStack.empty())
    return;

  _redoStack.push(_img);
  if (_redoStack.size() == 1)
    emit redoAvailable(true);

  _img = _undoStack.pop();
  _saved = false;
  if (_undoStack.empty())
    emit undoAvailable(false);

  update();
}

void DrawPane::redo()
{
  if (_redoStack.empty())
    return;

  push_undo();

  _img = _redoStack.pop();
  _saved = false;
  if (_redoStack.empty())
    emit redoAvailable(false);

  update();
}

void DrawPane::push_undo()
{
  _undoStack.push(_img);
  if (_undoStack.size() == 1)
    emit undoAvailable(true);
}

void DrawPane::start_action()
{
  if (!_actionStart)
    return;

  _saved = false;
  _redoStack.clear();
  push_undo();
  _actionStart = false;
}

void DrawPane::action_done()
{
  _actionStart = true;
  emit imageChanged();
}

void DrawPane::draw(const QPoint &p, int index)
{
  _img.setPixel(p, index);
}

void DrawPane::fill(const QPoint &p, int c, int t)
{
  if (!_img.rect().contains(p) || _img.pixelIndex(p) == c || _img.pixelIndex(p) != t)
    return;

  _img.setPixel(p, c);
  fill(QPoint(p.x() + 1, p.y()), c, t);
  fill(QPoint(p.x() - 1, p.y()), c, t);
  fill(QPoint(p.x(), p.y() + 1), c, t);
  fill(QPoint(p.x(), p.y() - 1), c, t);
}

void DrawPane::leftAction()
{
  if (!_img.rect().contains(_pixel))
    return;

  if (_currentMode == Draw)
  {
    start_action();
    draw(_pixel, _currentColorIndex);
  }
  else if (_currentMode == PickColor)
  {
    _currentColorIndex = _img.pixelIndex(_pixel);
    if (_onFished)
    {
      _onFished();
      _onFished = nullptr;
    }
  }
  else if (_currentMode == FillColor)
  {
    start_action();
    fill(_pixel, _currentColorIndex, _img.pixelIndex(_pixel));
    if (_onFished)
    {
      _onFished();
      _onFished = nullptr;
    }
  }
}

void DrawPane::rightAction()
{
  if (!_img.rect().contains(_pixel))
    return;

  if (_currentMode == Draw)
  {
    start_action();
    draw(_pixel, 0);
  }
  else if (_currentMode == FillColor)
  {
    start_action();
    fill(_pixel, 0, _img.pixelIndex(_pixel));
    if (_onFished)
    {
      _onFished();
      _onFished = nullptr;
    }
  }
}

void DrawPane::paintEvent(QPaintEvent *pe)
{
  if (_transform.isIdentity())
  {
    const auto s = qreal(std::min(width(), height())) / 1.1 / qreal(_img.height());
    _transform.scale(s, s);
    _transform.translate(width() / s / 2 - _img.width() / 2.0,
                         height() / s / 2.0 - _img.height() / 2.0);
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

  if (_img.rect().contains(_pixel))
  {
    p.setPen(QPen(Qt::lightGray, 0.0));
    p.drawRect(QRect(_pixel, QSize(1, 1)));
  }

  p.setTransform(QTransform());
  p.setBrush(Qt::lightGray);
  p.setPen(Qt::transparent);
  p.drawRect(QRect(QPoint(width() - _img.width() - 10, 0), _img.size() + QSize(10, 10)));
  p.setBrush(Qt::white);
  p.drawRect(QRect(QPoint(width() - _img.width() - 5, 5), _img.size()));

  p.drawImage(width() - _img.width() - 5, 5, _img);

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

  if (me->buttons() == Qt::MiddleButton)
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

void DrawPane::mouseReleaseEvent(QMouseEvent *me)
{
  action_done();
  QWidget::mouseReleaseEvent(me);
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
