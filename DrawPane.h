#pragma once

#include <QStack>
#include <QWidget>
#include <functional>

class DrawPane : public QWidget
{
  Q_OBJECT

  QImage _img;
  QStack<QImage> _undoStack;
  QStack<QImage> _redoStack;

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
  bool _actionStart{true};

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

  const QImage &currentImage() const { return _img; }

public slots:
  void undo();
  void redo();

  void newImage();

signals:
  void undoAvailable(bool);
  void redoAvailable(bool);

private:
  void push_undo();
  void start_action();

  void draw(const QPoint &p, const QColor &c);
  void leftAction();
  void rightAction();

  void paintEvent(QPaintEvent *pe);
  void mousePressEvent(QMouseEvent *me);
  void mouseMoveEvent(QMouseEvent *me);
  void mouseReleaseEvent(QMouseEvent *me);
  void wheelEvent(QWheelEvent *we);
};
