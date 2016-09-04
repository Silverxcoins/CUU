#include "exitdialog.h"
#include "ui_exitdialog.h"

ExitDialog::ExitDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ExitDialog) {
    ui->setupUi(this);
}

ExitDialog::~ExitDialog() {
    delete ui;
}

void ExitDialog::on_okButton_clicked() {
    this->accept();
}

void ExitDialog::on_cancelButton_clicked() {
    this->reject();
}
