#include "NewImageDialog.h"

#include "ui_NewImageDialog.h"

NewImageDialog::NewImageDialog(QWidget *parent) :
    QWidget{parent}, ui{std::make_unique<Ui::NewImageDialog>()}
{
  ui->setupUi(this);
}

NewImageDialog::~NewImageDialog() = default;

void NewImageDialog::onAppy(const ApplyCB &cb)
{
  _onApply = cb;
}

void NewImageDialog::on_pbOk_clicked()
{
  if (_onApply)
    _onApply(QSize(ui->spWidth->value(), ui->spHeight->value()));
}
