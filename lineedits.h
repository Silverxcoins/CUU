#ifndef LINEEDITS_H
#define LINEEDITS_H
#include <QObject>
#include <QLineEdit>
#include <QKeyEvent>
#include <QTableWidget>

class TTLineEdit : public QLineEdit {
    Q_OBJECT

public:
    TTLineEdit(int row, int column);

    ~TTLineEdit();

    void keyPressEvent(QKeyEvent *event);

private slots:
    void onTransitionConditionsChanged(const QString &string);

    void onControlSignalsChanged(const QString &string);

private:
    int row;

    int column;

    bool isCorrect;

    void setIsCorrect(bool isCorrect);
};

class OPLineEdit : public QLineEdit {
    Q_OBJECT

public:
    OPLineEdit(QTableWidget *table, int row, int column);

    ~OPLineEdit();

    void keyPressEvent(QKeyEvent *event);

private slots:
    void onTextChanged(const QString &string);

private:
    QTableWidget *table;

    int row;

    int column;
};

#endif // LINEEDITS_H
