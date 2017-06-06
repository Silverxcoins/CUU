#ifndef INFODIALOG_H
#define INFODIALOG_H

#include "executionwindow.h"

#include <QDialog>
#include <QString>

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit InfoDialog(ExecutionWindow *executionWindow,
                        QString infoText,
                        bool insert = false,
                        QWidget *parent = 0);

    ~InfoDialog();

    void keyPressEvent(QKeyEvent*);
    
private:
    Ui::InfoDialog *ui;

    ExecutionWindow *executionWindow;

    bool insert;
};

#endif // INFODIALOG_H
