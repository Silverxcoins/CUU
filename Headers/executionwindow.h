#ifndef EXECUTIONWINDOW_H
#define EXECUTIONWINDOW_H

#include "mainwindow.h"

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class ExecutionWindow;
}

class ExecutionWindow : public QDialog {
    Q_OBJECT

public:
    explicit ExecutionWindow(MainWindow *mainWindow, QWidget *parent = 0);

    ~ExecutionWindow();

    void closeEvent(QCloseEvent *);

    void keyPressEvent(QKeyEvent *);

    void setA(int A);

private slots:
    void on_nextButton_pressed();

    void on_nextButton_clicked();

    void on_endButton_pressed();

    void on_endButton_clicked();

private:
    Ui::ExecutionWindow *ui;

    void moveToNextState();

    void moveToNextState(int firstRow);

    void executeNextOperation();

    int executeAluOperation(int x, QString xName, int y, QString yName);

    bool areConditionsSatisfied();

    int readFromRam(int address);

    void readRk(int commandAdressesNumber);

    void writeToRam();

    int calculateT();

    int bitwiseNot(int x);

    int bitwiseLeftShift(int x, bool valueToInsert);

    int bitwiseRightShift(int x, bool valueToInsert);

    void checkCarryAndOverflow(int x, int y,
                               bool isOperationSubstraction = false);

    void printValues(QString location);

    void hideValues();

    QString toHexString(int number);

    bool *toBinArray(int number);

    int fromBinArray(bool *binArray);

    MainWindow *mainWindow;

    bool nextTransitionExists;

    bool areConditionsDescribed;

    bool isProgramStopped;

    bool isProgramFinished;

    bool isProgramOnRewindToEnd;

    int step;

    int row;

    int initialState;

    int nextState;

    QString conditionsText;

    QString signalsText;

    QString operationString;

    int *RK;

    int *RON;

    int A;

    int B;

    int RS;

    int SP;

    int RI;

    int RA;

    int RV;

    int R;

    int S1;

    int S2;

    int S3;

    bool X4;

    bool X3;

    bool X2;

    bool X1;

    bool Z;

    bool N;

    bool C;

    bool P;

    int *controlSignals;

    int *signalsFlags;

    int currentSignalNumber;
};

#endif // EXECUTIONWINDOW_H
