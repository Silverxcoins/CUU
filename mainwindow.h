#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTableWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();
    
private slots:
    void addString(int stringNumber);

    void on_editButton_clicked();

    void on_OPButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
