#ifndef RAMLINEEDIT_H
#define RAMLINEEDIT_H
#include <QObject>
#include <QLineEdit>
#include <QTableWidget>
#include <QKeyEvent>


class RamLineEdit : public QLineEdit {
    Q_OBJECT

public:
    RamLineEdit(QTableWidget *table, int row, int column);

    ~RamLineEdit();

    void keyPressEvent(QKeyEvent *event);

private slots:
    void onTextChanged(const QString &string);

private:
    QTableWidget *table;

    int row;

    int column;
};

#endif // RAMLINEEDIT_H
