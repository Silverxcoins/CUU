#include <errorswindow.h>

ErrorsWindow::ErrorsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new ErrorsWindow)
{
    ui->setupUi(this);
}

ErrorsWindow::~ErrorsWindow() {
    delete ui;
}
