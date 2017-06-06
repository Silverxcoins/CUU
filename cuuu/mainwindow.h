#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ramlineedit.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QTableWidget>
#include <QGridLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void closeEvent(QCloseEvent*);

    QTableWidget *getRamTable();

    QLineEdit *getTtField(int row, int column);

    RamLineEdit *getRamCell(int row, int column);

    int getStringCounter();

    void setFocus(int rowInFocus, int columnInFocus);

    int getRowInFocus();

    int getColumnInFocus();

    void incrementNErrors();

    void decrementNErrors();

    void deleteString();

signals:
    void closed();
    
private slots:
    void on_editButton_clicked();

    void on_ramButton_clicked();

    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_startButton_clicked();

    void on_startButton_pressed();

    void on_actionOpen_activated();

    void on_actionSave_activated();

    void on_actionNew_activated();

    void addString(int stringNumber);

    void allowTtChangesAndLaunch();

private:
    void appDataFromText(QString data);

    QString appDataToText();

    void clearGrid();

    void rejectTtChangesAndLaunch();

    Ui::MainWindow *ui;

    QGridLayout *grid;

    int stringCounter;

    int rowInFocus;

    int columnInFocus;

    int nErrors;

    bool isLastStringNeeded;

    QString saveDir;

    QString openDir;

};

#endif // MAINWINDOW_H
