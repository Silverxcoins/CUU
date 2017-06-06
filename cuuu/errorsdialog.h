#ifndef ERRORSDIALOG_H
#define ERRORSDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class ErrorsDialog;
}

class ErrorsDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit ErrorsDialog(QString error, QString info, QWidget *parent = 0);

    ~ErrorsDialog();
    
private slots:
    void on_okButton_pressed();

    void on_okButton_released();

    void on_okButton_clicked();

private:
    Ui::ErrorsDialog *ui;

};

#endif // ERRORSDIALOG_H
