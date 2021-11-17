#pragma once

#include <QWidget>
#include <functional>
#include <memory>

namespace Ui
{
class NewImageDialog;
}

class NewImageDialog : public QWidget
{
  Q_OBJECT

  using ApplyCB = std::function<void(const QSize &)>;
  using CancelCB = std::function<void()>;

  std::unique_ptr<Ui::NewImageDialog> ui;

  ApplyCB _onApply;
  CancelCB _onCancel;

public:
  explicit NewImageDialog(QWidget *parent = nullptr);
  ~NewImageDialog();

  void onAppy(const ApplyCB &cb);
  void onCancel(const CancelCB &cb);

private slots:
  void on_pbOk_clicked();
  void on_pbCancel_clicked();
};
