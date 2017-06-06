#include "infodialog.h"
#include "ui_infodialog.h"
#include "executionwindow.h"
#include "constants.h"

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTextCodec>

InfoDialog::InfoDialog(ExecutionWindow *executionWindow,
                       QString infoText,
                       bool insert,
                       QWidget *parent) :
        QDialog(parent),
        ui(new Ui::InfoDialog) {
    QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
    ui->setupUi(this);
    this->executionWindow = executionWindow;
    this->insert = insert;
    ui->infoLabel->setText(infoText);
    if (!insert) {
        ui->pressKeyLabel->setText(tr("Для продолжения нажмите любую клавишу"));
        ui->byteLineEdit->hide();
    } else {
        ui->pressKeyLabel->setText(tr("Для продолжения нажмите Enter"));
        ui->infoLabel->setAlignment(Qt::AlignLeft);
        ui->infoLabel->setContentsMargins(21,11,0,0);
        ui->byteLineEdit->setInputMask(">HH");
    }
}

InfoDialog::~InfoDialog() {
    delete ui;
}

void InfoDialog::keyPressEvent(QKeyEvent *event) {
    if (insert && event->key() == Qt::Key_Return) {
        int A = ui->byteLineEdit->text().toInt(0, HEX);
        executionWindow->setA(A);
        this->close();
    } else if (!insert){
        this->close();
    }
}
