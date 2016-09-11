#include "errorsdialog.h"
#include "ui_errorsdialog.h"

ErrorsDialog::ErrorsDialog(QString error, QString info, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ErrorsDialog) {
    ui->setupUi(this);
    ui->errorLabel->setText(error);
    ui->infoLabel->setText(info);
}

ErrorsDialog::~ErrorsDialog() {
    delete ui;
}

void ErrorsDialog::on_okButton_pressed() {
    ui->okButton->move(ui->okButton->x() - 1,
                       ui->okButton->y() - 1);
    ui->okButton->resize(ui->okButton->width() - 1,
                         ui->okButton->height() - 1);
}

void ErrorsDialog::on_okButton_released() {
    ui->okButton->move(ui->okButton->x() + 1,
                       ui->okButton->y() + 1);
    ui->okButton->resize(ui->okButton->width() + 1,
                         ui->okButton->height() + 1);
}

void ErrorsDialog::on_okButton_clicked() {
    this->accept();
}
