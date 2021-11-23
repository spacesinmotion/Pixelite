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

  int _currentColorIndex;

  enum Mode
  {
    Draw,
    PickColor,
    FillColor,
  };
  Mode _currentMode{Draw};
  bool _actionStart{true};
  bool _saved{true};

  using CB = std::function<void()>;
  CB _onFished;

public:
  explicit DrawPane(QWidget *parent = nullptr);

  QColor currentColor() { return _img.color(_currentColorIndex); }
  void setCurrentColor(const QColor &c);
  QVector<QRgb> calc_color_table() const;
  QVector<QRgb> get_color_table() const;

  void drawMode() { _currentMode = Draw; }
  void pickColorMode(const CB &onFinish)
  {
    _currentMode = PickColor;
    _onFished = onFinish;
  }
  void fillColorMode(const CB &onFinish)
  {
    _currentMode = FillColor;
    _onFished = onFinish;
  }

  const QImage &currentImage() const { return _img; }
  void setCurrentImage(const QImage &img);
  void mark_saved() { _saved = true; }
  bool has_changed() const { return !_saved; }

  void newImage(const QSize &s);

public slots:
  void undo();
  void redo();

signals:
  void undoAvailable(bool);
  void redoAvailable(bool);
  void imageChanged();

private:
  void push_undo();
  void start_action();
  void action_done();

  void draw(const QPoint &p, int index);
  void fill(const QPoint &p, int c, int t);
  void leftAction();
  void rightAction();

  void paintEvent(QPaintEvent *pe) final;
  void mousePressEvent(QMouseEvent *me) final;
  void mouseMoveEvent(QMouseEvent *me) final;
  void mouseReleaseEvent(QMouseEvent *me) final;
  void wheelEvent(QWheelEvent *we) final;
};
