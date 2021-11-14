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

  std::unique_ptr<Ui::NewImageDialog> ui;

  ApplyCB _onApply;

public:
  explicit NewImageDialog(QWidget *parent = nullptr);
  ~NewImageDialog();

  void onAppy(const ApplyCB &cb);

private slots:
  void on_pbOk_clicked();
};
