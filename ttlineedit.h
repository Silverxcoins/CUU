#ifndef TTLINEEDIT_H
#define TTLINEEDIT_H
#include <QObject>
#include <QLineEdit>
#include <QKeyEvent>
#include <QString>

#include <mainwindow.h>

class TTLineEdit : public QLineEdit {
    Q_OBJECT

public:
    TTLineEdit(MainWindow *mainWindow, int row, int column);

    ~TTLineEdit();

    void keyPressEvent(QKeyEvent *event);

    void focusInEvent(QFocusEvent *event);

    void emitFocusOut();

    void deleteField();

    bool getIsCorrect();

    void setIsCorrect(bool isCorrect);

    int *getConditionsNumbers();

    bool *getConditionsSigns();

    int *getControlSignals();

private slots:
    void onStateEditingFinished();

    void onStateChanged(const QString &);

    void onTransitionConditionsEditingFinished();

    void onTransitionConditionsChanged(const QString &);

    void onControlSignalsEditingFinished();

    void onControlSignalsChanged(const QString &);

private:
    bool areTransitionConditionsCorrect();

    void sortTransitionConditions();

    bool areControlSignalsCorrect();

    void sortControlSignals();

    void insertionSort(int *array, int stopNumber);

    void insertionSort(int *array, QChar *signs, int stopNumber);

    MainWindow *mainWindow;

    int row;

    int column;

    bool isCorrect;

    int *conditionsNumbers;

    QChar *conditionsSignsSymbols;

    bool *conditionsSigns;

    int *controlSignals;
};

#endif // TTLINEEDIT_H
