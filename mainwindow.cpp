#include "mainwindow.h"
#include "lineedits.h"
#include "statevalidator.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QKeyEvent>
#include <QSignalMapper>
#include <QScrollBar>
#include <QColor>

QGridLayout *grid;
int stringCounter = 0;
int errors = 0;

const char* OP_EVEN_STYLE = "QLineEdit {border: 0px; background-color: rgb(250,250,250); color: rgb(75,75,75); font-weight: bold;} QLineEdit:focus{border: 1px solid rgb(100,100,100);}";
const char* OP_ODD_STYLE= "QLineEdit {border: 0px solid; color: rgb(75,75,75); font-weight: bold;} QLineEdit:focus {border: 1px solid rgb(100,100,100);}";
const char* TT_LINE_EDIT_STYLE = "background-color: #f2f2f2; border-radius: 6px; font: 10pt \"Courier\"; font-weight: bold; color: #4C5866;";
const char* TT_LINE_EDIT_ERROR_STYLE = "background-color: #f2f2f2; border-radius: 6px; border: 1px solid #E28383; font: 10pt \"Courier\"; font-weight: bold; color: #4C5866;";
const char* STRING_NUMBER_LABEL_STYLE = "background-color: rgb(101, 220, 177); border-radius: 10px; color: rgb(255, 255, 255);";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->OPButton->setStyleSheet("background-color: rgb(234, 234, 234); border: 1px solid  rgb(217, 217, 217)");
    ui->OPTable->hide();
    ui->OPTableDecor->hide();
    for (int pos = 0; pos < 15; pos++) {
        ui->OPVLayout->itemAt(pos)->widget()->hide();
        ui->OPHLayout->itemAt(pos)->widget()->hide();
        ((QLabel*)ui->OPHLayout->itemAt(pos)->widget())->setStyleSheet("border-radius: 8px; background-color: rgb(87, 161, 217); color: white; font-weight: bold;");
        ((QLabel*)ui->OPVLayout->itemAt(pos)->widget())->setStyleSheet("border-radius: 8px; background-color: rgb(87, 161, 217); color: white; font-weight: bold;");
    }

    grid = new QGridLayout();
    ui->scrollAreaContents->setLayout(grid);

    QSpacerItem *vSpacer = new QSpacerItem(1, 200, QSizePolicy::Maximum, QSizePolicy::Maximum);
    grid->addItem(vSpacer, stringCounter, 1, 18, 1);

    addString(stringCounter);

    for (int row = 0; row < 15; row++)
        ui->OPTable->setRowHeight(row, 23);
    for (int column = 0; column < 15; column++)
        ui->OPTable->setColumnWidth(column, 35);

    OPLineEdit *OPCells[15][15];
    for (int row = 0; row < 15; row++)
        for (int column = 0; column < 15; column++) {
            OPCells[row][column] = new OPLineEdit(ui->OPTable, row, column);
        }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::addString(int stringNumber) {
    if (stringNumber < stringCounter) {
        addString(stringCounter);

        QLineEdit *lineEditTo = new QLineEdit();
        QLineEdit *lineEditFrom = new QLineEdit();
        for (int counter = stringCounter; counter > stringNumber; counter--) {
            for (int column = 8; column >= 2; column -= 2) {
            lineEditTo = (QLineEdit *)grid->itemAtPosition(counter - 1, column)->widget();
            lineEditFrom = (QLineEdit *)grid->itemAtPosition(counter - 2, column)->widget();
            lineEditTo->setText(lineEditFrom->text());
            }
        }
        for (int column = 8; column >= 2; column -= 2) {
            lineEditTo = (QLineEdit *)grid->itemAtPosition(stringNumber, column)->widget();
            lineEditTo->clear();
        }
        lineEditTo->setFocus();
    } else {
        // Номер строки
        QLabel *stringNumberLabel = new QLabel(QString::number(stringNumber + 1));
        stringNumberLabel->setMinimumWidth(20);
        stringNumberLabel->setAlignment(Qt::AlignCenter);
        stringNumberLabel->setStyleSheet(STRING_NUMBER_LABEL_STYLE);

        // Спейсеры
        QSpacerItem *hSpacer1 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
        QSpacerItem *hSpacer2 = new QSpacerItem(8, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
        QSpacerItem *hSpacer3 = new QSpacerItem(8, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
        QSpacerItem *hSpacer4 = new QSpacerItem(9, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);

        // Исходное состояние
        QLineEdit *initialStateLineEdit = new TTLineEdit(stringNumber, 0);
        initialStateLineEdit->setMaximumWidth(86);
        initialStateLineEdit->setAlignment(Qt::AlignCenter);
        initialStateLineEdit->setMaxLength(3);
        initialStateLineEdit->setValidator(new StateValidator());

        // Следующее состояние
        QLineEdit *nextStateLineEdit = new TTLineEdit(stringNumber, 1);
        nextStateLineEdit->setMaximumWidth(86);
        nextStateLineEdit->setAlignment(Qt::AlignCenter);
        nextStateLineEdit->setMaxLength(3);
        nextStateLineEdit->setValidator(new StateValidator());

        // Условия перехода
        QLineEdit *transitionConditionsLineEdit = new TTLineEdit(stringNumber, 2);
        transitionConditionsLineEdit->setMaximumWidth(186);
        transitionConditionsLineEdit->setTextMargins(5,0,5,0);
        MainWindow::connect(transitionConditionsLineEdit, SIGNAL(textChanged(const QString &)),
                            transitionConditionsLineEdit, SLOT(onTransitionConditionsChanged(const QString &)));

        // Управляющие сигналы
        QLineEdit *controlSignalsLineEdit = new TTLineEdit(stringNumber, 3);
        controlSignalsLineEdit->setTextMargins(5,0,5,0);
        MainWindow::connect(controlSignalsLineEdit, SIGNAL(textChanged(const QString &)),
                            controlSignalsLineEdit, SLOT(onControlSignalsChanged(const QString &)));

        grid->addWidget(stringNumberLabel, stringNumber, 0);
        grid->addItem(hSpacer1, stringNumber, 1);
        grid->addWidget(initialStateLineEdit, stringNumber, 2);
        grid->addItem(hSpacer2, stringNumber, 3);
        grid->addWidget(nextStateLineEdit, stringNumber, 4);
        grid->addItem(hSpacer3, stringNumber, 5);
        grid->addWidget(transitionConditionsLineEdit, stringNumber, 6);
        grid->addItem(hSpacer4, stringNumber, 7);
        grid->addWidget(controlSignalsLineEdit, stringNumber, 8);

        initialStateLineEdit->setFocus();

        QSignalMapper* signalMapper = new QSignalMapper(this) ;
        MainWindow::connect(initialStateLineEdit, SIGNAL(returnPressed()), signalMapper, SLOT(map()));
        MainWindow::connect(nextStateLineEdit, SIGNAL(returnPressed()), signalMapper, SLOT(map()));
        MainWindow::connect(transitionConditionsLineEdit, SIGNAL(returnPressed()), signalMapper, SLOT(map()));
        MainWindow::connect(controlSignalsLineEdit, SIGNAL(returnPressed()), signalMapper, SLOT(map()));

        signalMapper -> setMapping (initialStateLineEdit, stringNumber + 1) ;
        signalMapper -> setMapping (nextStateLineEdit, stringNumber + 1) ;
        signalMapper -> setMapping (transitionConditionsLineEdit, stringNumber + 1) ;
        signalMapper -> setMapping (controlSignalsLineEdit, stringNumber + 1) ;

        connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(addString(int)));
        stringCounter++;
    }
}

void MainWindow::on_editButton_clicked() {
    ui->editButton->setStyleSheet("border: 1px solid  rgb(217, 217, 217)");
    ui->OPButton->setStyleSheet("background-color: rgb(234, 234, 234); border: 1px solid  rgb(217, 217, 217)");
    ui->OPTable->hide();
    ui->OPTableDecor->hide();
    for (int pos = 0; pos < 15; pos++) {
        ui->OPVLayout->itemAt(pos)->widget()->hide();
        ui->OPHLayout->itemAt(pos)->widget()->hide();
    }
    ui->label1->show();
    ui->label2->show();
    ui->label3->show();
    ui->label4->show();
    ui->line1->show();
    ui->line2->show();
    ui->line3->show();
    ui->line4->show();
    ui->scrollArea->show();
}

void MainWindow::on_OPButton_clicked() {
    ui->OPButton->setStyleSheet("border: 1px solid  rgb(217, 217, 217)");
    ui->editButton->setStyleSheet("background-color: rgb(234, 234, 234); border: 1px solid  rgb(217, 217, 217)");
    ui->label1->hide();
    ui->label2->hide();
    ui->label3->hide();
    ui->label4->hide();
    ui->line1->hide();
    ui->line2->hide();
    ui->line3->hide();
    ui->line4->hide();
    ui->scrollArea->hide();
    ui->OPTable->show();
    ui->OPTableDecor->show();
    for (int pos = 0; pos < 15; pos++) {
        ui->OPVLayout->itemAt(pos)->widget()->show();
        ui->OPHLayout->itemAt(pos)->widget()->show();
    }
}

TTLineEdit::TTLineEdit(int row, int column) : QLineEdit() {
    this->row = row;
    this->column = (column + 1) * 2;
    this->setStyleSheet(TT_LINE_EDIT_STYLE);
    this->isCorrect = true;
}

TTLineEdit::~TTLineEdit() {}

void TTLineEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W || event->key() == Qt::Key_Up) {
        if (row > 0)
            grid->itemAtPosition(row - 1, column)->widget()->setFocus();
    }
    else if(event->key() == Qt::Key_S || event->key() == Qt::Key_Down) {
        if (row < stringCounter - 1)
            grid->itemAtPosition(row + 1, column)->widget()->setFocus();
    }
    else if(event->key() == Qt::Key_A || (event->key() == Qt::Key_Left && this->cursorPosition() == 0)) {
        if (column > 2)
            grid->itemAtPosition(row, column - 2)->widget()->setFocus();
    }
    else if(event->key() == Qt::Key_D || (event->key() == Qt::Key_Right && this->cursorPosition() == this->text().length())) {
        if (column < 8)
            grid->itemAtPosition(row, column + 2)->widget()->setFocus();
    }
    else{
        QLineEdit::keyPressEvent(event);
    }
}

void TTLineEdit::onTransitionConditionsChanged(const QString &data) {
    if (data.length() == 0) {
        this->setIsCorrect(true);
        return;
    }

    bool isDataWrong = false;
    char c;
    if (data.length() == 1 || data.length() == 2) {
        c = (data.length() == 1) ? data[0].toAscii() : data[1].toAscii();
        isDataWrong = (c != 'b' && c != 'B' && c != 'z' && c != 'Z' && c != 'n' && c != 'N' && c != 'c' && c != 'C' && c != 'p' && c != 'P');
        isDataWrong = (data.length() == 1) ? isDataWrong : (isDataWrong || (data[0].toAscii() != '-' && data[0].toAscii() != '!'));
        if (!isDataWrong) {
            this->setIsCorrect(true);
            return;
        }
    }

    char currentCondition[4];
    int commonCounter = 0;
    int conditionsCounter = 0;
    bool isDataEnded = false;
    isDataWrong = false;
    while (!isDataEnded && !isDataWrong) {
        if (conditionsCounter == 4) isDataWrong = true;

        int counter = 0;
        do {
            if (counter == 4) {
                isDataWrong = true;
                break;
            }
            c = data[commonCounter++].toAscii();
            if (c != ' ') currentCondition[counter++] = c;
            if (commonCounter == data.length()) {
                isDataEnded = true;
                break;
            }
        } while (c != ',');

        if (currentCondition[0] != 'x' && currentCondition[0] != 'X') {
            if ((currentCondition[0] != '-' && currentCondition[0] != '!')
                 || (currentCondition[1] != 'x' && currentCondition[1] != 'X')
                 || currentCondition[2] < '1'
                 || currentCondition[2] > '4'
                 || (isDataEnded
                     && c != ','
                     && c != ' '
                     && (counter == 4 || c < '1' || c > '4'))) {
                isDataWrong = true;
            }
        } else {
            if (currentCondition[1] < '1'
                || currentCondition[1] > '4'
                || (isDataEnded
                    && c != ','
                    && c != ' '
                    && (counter == 3 || c < '1' || c > '4'))) isDataWrong = true;
        }
        conditionsCounter++;
    }

    if (isDataWrong) {
        this->setIsCorrect(false);
    } else {
        this->setIsCorrect(true);
    }
}

void TTLineEdit::onControlSignalsChanged(const QString &data) {
    if (data.length() == 0) {
        this->setIsCorrect(true);
        return;
    }

    char currentSignal[4];
    char c;
    int commonCounter = 0;
    bool isDataWrong = false;
    bool isDataEnded = false;
    while (!isDataEnded && !isDataWrong) {
        int counter = 0;
        do {
            if (commonCounter == data.length()) {
                isDataEnded = true;
                break;
            }
            if (counter == 4) {
                isDataWrong = true;
                break;
            }
            c = data[commonCounter++].toAscii();
            if (c != ' ') currentSignal[counter++] = c;
        } while (c != ',');

        if (currentSignal[0] != 'y' && currentSignal[0] != 'Y') isDataWrong = true;
        if (isDataEnded && c != ',' && c != ' ' && (counter == 4 || c < '0' || c > '9')) isDataWrong = true;
        for (int pos = 1; pos < counter - 1; pos++) {
            if (currentSignal[pos] < '0' || currentSignal[pos] > '9') isDataWrong = true;
        }
    }

    if (isDataWrong) {
        this->setIsCorrect(false);
    } else {
        this->setIsCorrect(true);
    }
}

void TTLineEdit::setIsCorrect(bool isCorrect) {
    if (isCorrect) {
        if (!this->isCorrect) {
            this->setStyleSheet(TT_LINE_EDIT_STYLE);
            this->isCorrect = true;
            errors--;
        }
    } else {
        this->setStyleSheet(TT_LINE_EDIT_ERROR_STYLE);
        if (this->isCorrect) {
            this->isCorrect = false;
            errors++;
        }
    }
}

OPLineEdit::OPLineEdit(QTableWidget *table, int row, int column) : QLineEdit() {
    this->table = table;
    this->row = row;
    this->column = column;
    this->setText("00");
    this->setAlignment(Qt::AlignHCenter);
    this->setInputMask(">HH");
    if (row % 2 == 0)
        this->setStyleSheet(OP_EVEN_STYLE);
    else
        this->setStyleSheet(OP_ODD_STYLE);
    MainWindow::connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));
    table->setCellWidget(row, column, this);
}

OPLineEdit::~OPLineEdit() {}

void OPLineEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up) {
        if (row > 0) {
            ((OPLineEdit*)table->cellWidget(row - 1, column))->setCursorPosition(this->cursorPosition());
            ((OPLineEdit*)table->cellWidget(row - 1, column))->setFocus();
        }
    }
    else if (event->key() == Qt::Key_Down) {
        if (row < 14) {
            ((OPLineEdit*)table->cellWidget(row + 1, column))->setCursorPosition(this->cursorPosition());
            ((OPLineEdit*)table->cellWidget(row + 1, column))->setFocus();
        }
    }
    else if (event->key() == Qt::Key_Left) {
        if (column > 0 && this->cursorPosition() == 0) {
            ((OPLineEdit*)table->cellWidget(row, column - 1))->setCursorPosition(1);
            ((OPLineEdit*)table->cellWidget(row, column - 1))->setFocus();
        } else {
            QLineEdit::keyPressEvent(event);
        }
    }
    else if (event->key() == Qt::Key_Right) {
        if (column < 14 && this->cursorPosition() > 0) {
            ((OPLineEdit*)table->cellWidget(row, column + 1))->setCursorPosition(0);
            ((OPLineEdit*)table->cellWidget(row, column + 1))->setFocus();
        } else {
            QLineEdit::keyPressEvent(event);
        }
    }
    else if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        this->insert("0");
    }
    else{
        QLineEdit::keyPressEvent(event);
    }
}

void OPLineEdit::onTextChanged(const QString &q)
{
    if (this->cursorPosition() == 2) {
        if (column < 14) {
            ((OPLineEdit*)table->cellWidget(row, column + 1))->setCursorPosition(0);
            ((OPLineEdit*)table->cellWidget(row, column + 1))->setFocus();
        } else if (row < 14) {
            ((OPLineEdit*)table->cellWidget(row + 1, 0))->setCursorPosition(0);
            ((OPLineEdit*)table->cellWidget(row + 1, 0))->setFocus();
        }
    }
}

StateValidator::StateValidator() : QValidator() {}

StateValidator::~StateValidator() {}

QValidator::State StateValidator::validate(QString &input, int &pos) const {
    if (input.isEmpty()) return QValidator::Acceptable;

    bool b;
    int value = input.toInt(&b);
    if ((b == true) && (value >= 0) && (value <= 999))
    {
        return Acceptable;
    }

    return Invalid;
}
