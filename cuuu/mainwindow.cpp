#include "mainwindow.h"
#include "ttlineedit.h"
#include "ramlineedit.h"
#include "statevalidator.h"
#include "ui_mainwindow.h"
#include "styles.h"
#include "errorsdialog.h"
#include "executionwindow.h"
#include "constants.h"
#include "exitdialog.h"

#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QKeyEvent>
#include <QSignalMapper>
#include <QScrollBar>
#include <QColor>
#include <QFileDialog>
#include <QTextStream>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));

    stringCounter = 0;
    rowInFocus = 0;
    columnInFocus = INITIAL_STATE_COLUMN;
    nErrors = 0;
    isLastStringNeeded = false;
    saveDir = QDir::homePath();
    openDir = QDir::homePath();

    ui->ramButton->setStyleSheet(BUTTON_UNCHECKED_STYLE);
    ui->ramTable->hide();
    ui->ramTableDecor->hide();
    for (int pos = 0; pos < RAM_ROWS_COUNT; pos++) {
        ui->ramVLayout->itemAt(pos)->widget()->hide();
        ((QLabel*)ui->ramVLayout->itemAt(pos)->widget())
                    ->setStyleSheet(RAM_LABEL_STYLE);
    }

    for (int pos = 0; pos < RAM_COLUMNS_COUNT; pos++) {
        ui->ramHLayout->itemAt(pos)->widget()->hide();
        ((QLabel*)ui->ramHLayout->itemAt(pos)->widget())
                    ->setStyleSheet(RAM_LABEL_STYLE);
    }

    grid = new QGridLayout();
    ui->scrollAreaContents->setLayout(grid);

    QSpacerItem *vSpacer = new QSpacerItem(1, 200,
                                           QSizePolicy::Fixed,
                                           QSizePolicy::Fixed);
    grid->addItem(vSpacer, stringCounter, 1, 18, 1);

    addString(stringCounter);

    for (int row = 0; row < RAM_ROWS_COUNT; row++)
        ui->ramTable->setRowHeight(row, 23);
    for (int column = 0; column < RAM_COLUMNS_COUNT; column++)
        ui->ramTable->setColumnWidth(column, 35);

    RamLineEdit *ramCells[RAM_ROWS_COUNT][RAM_COLUMNS_COUNT];
    for (int row = 0; row < RAM_ROWS_COUNT; row++)
        for (int column = 0; column < RAM_COLUMNS_COUNT; column++) {
            ramCells[row][column] = new RamLineEdit(ui->ramTable, row, column);
        }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    ExitDialog *exitDialog = new ExitDialog();
    if ((*exitDialog).exec() == QDialog::Rejected) {
        event->ignore();
    } else {
        emit closed();
    }
    delete exitDialog;
}

void MainWindow::on_actionNew_activated() {
    clearGrid();
    stringCounter = 0;
    nErrors = 0;
    addString(stringCounter);
    rowInFocus = 0;
    columnInFocus = INITIAL_STATE_COLUMN;

    for (int row = 0; row < RAM_ROWS_COUNT; row++) {
        for (int column = 0; column < RAM_COLUMNS_COUNT; column++) {
            getRamCell(row,column)->setText("00");
        }
    }

    emit this->closed();
}

void MainWindow::on_actionOpen_activated() {
    ((TTLineEdit*)getTtField(rowInFocus,columnInFocus))->emitFocusOut();

    QString fileName = QFileDialog::getOpenFileName(this, "",
                                                    openDir, "*.cuu");
    if (!fileName.isEmpty()) {
        openDir = fileName;
    } else {
        return;
    }

    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&file);
        QString data = in.readAll();

        for (int pos = 0; pos < CONTROL_SEQUENCE.length(); pos++) {
            if (data[pos] != CONTROL_SEQUENCE[pos]) {
                return;
            }
        }

        appDataFromText(data);
        file.close();
        emit this->closed();
    }
}

void MainWindow::on_actionSave_activated() {
    ((TTLineEdit*)getTtField(rowInFocus,columnInFocus))->emitFocusOut();

    QString fileName = QFileDialog::getSaveFileName(this, "",
                                                    saveDir, "*.cuu");
    if (!fileName.isEmpty()) {
        saveDir = fileName;
    } else {
        return;
    }

    QFile file(fileName);
    if(file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << appDataToText();
        file.flush();
        file.close();
    }

}

void MainWindow::on_editButton_clicked() {
    ui->editButton->setStyleSheet(BUTTON_CHECKED_STYLE);
    ui->ramButton->setStyleSheet(BUTTON_UNCHECKED_STYLE);
    ui->ramTable->hide();
    ui->ramTableDecor->hide();
    for (int pos = 0; pos < RAM_ROWS_COUNT; pos++) {
        ui->ramVLayout->itemAt(pos)->widget()->hide();
    }
    for(int pos = 0; pos < RAM_COLUMNS_COUNT; pos++) {
        ui->ramHLayout->itemAt(pos)->widget()->hide();
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
    ui->addButton->show();
    ui->deleteButton->show();
}

void MainWindow::on_ramButton_clicked() {
    ui->ramButton->setStyleSheet(BUTTON_CHECKED_STYLE);
    ui->editButton->setStyleSheet(BUTTON_UNCHECKED_STYLE);
    ui->label1->hide();
    ui->label2->hide();
    ui->label3->hide();
    ui->label4->hide();
    ui->line1->hide();
    ui->line2->hide();
    ui->line3->hide();
    ui->line4->hide();
    ui->scrollArea->hide();
    ui->addButton->hide();
    ui->deleteButton->hide();

    ui->ramTable->show();
    ui->ramTableDecor->show();
    for (int pos = 0; pos < RAM_ROWS_COUNT; pos++) {
        ui->ramVLayout->itemAt(pos)->widget()->show();
    }
    for(int pos = 0; pos < RAM_COLUMNS_COUNT; pos++) {
        ui->ramHLayout->itemAt(pos)->widget()->show();
    }
}

void MainWindow::on_addButton_clicked() {
    addString(rowInFocus + 1);
}

void MainWindow::on_deleteButton_clicked() {
    deleteString();
}

void MainWindow::on_startButton_pressed() {
    ui->startButton->resize(ui->startButton->width() - 3,
                            ui->startButton->height() - 3);
}

void MainWindow::on_startButton_clicked() {
    ui->startButton->resize(ui->startButton->width() + 3,
                            ui->startButton->height() + 3);
    rejectTtChangesAndLaunch();
    if (nErrors == 0) {
        ExecutionWindow *executionWindow = new ExecutionWindow(this);
        connect(executionWindow, SIGNAL(destroyed()),
                this, SLOT(allowTtChangesAndLaunch()));
        (*executionWindow).show();
    } else {
        QString error = tr("Программа содержит ошибки");
        QString info = tr("Поля, содержащие ошибки, подсвечены красным.\n");
        info += tr("Исправьте данные и попробуйте снова.");
        ErrorsDialog *errorsDialog = new ErrorsDialog(error, info);
        connect(errorsDialog, SIGNAL(accepted()),
                this, SLOT(allowTtChangesAndLaunch()));
        connect(errorsDialog, SIGNAL(rejected()),
                this, SLOT(allowTtChangesAndLaunch()));
        (*errorsDialog).exec();
    }
}

void MainWindow::addString(int stringNumber) {
    if (stringCounter > MAX_ROWS_COUNT) {
        return;
    }

    bool isFirstPositionFirstString =
            rowInFocus == 0
            && columnInFocus == INITIAL_STATE_COLUMN
            && stringCounter > 0
            && getTtField(0,INITIAL_STATE_COLUMN)->cursorPosition() == 0;
    if (isFirstPositionFirstString) {
        if (getTtField(0,INITIAL_STATE_COLUMN)->text().isEmpty()) {
            for (int i = NEXT_STATE_COLUMN;
                 i <= CONTROL_SIGNALS_COLUMN;
                 i += 2) {
                if (!getTtField(0,i)->text().isEmpty()) {
                    ((TTLineEdit*)getTtField(0,INITIAL_STATE_COLUMN))
                            ->setIsCorrect(false);
                }
            }
        }
    }

    if (isFirstPositionFirstString
            && stringCounter == 1
            && stringNumber == 1
            && !isLastStringNeeded) {
        stringNumber--;
        isLastStringNeeded = true;

    }

    if (stringNumber < stringCounter) {
        addString(stringCounter);

        int lastMovingString = stringNumber + 1;
        if (isFirstPositionFirstString && stringNumber != 0) {
            lastMovingString--;
        }

        TTLineEdit *lineEditTo;
        TTLineEdit *lineEditFrom;
        for (int counter = stringCounter - 1;
             counter >= lastMovingString;
             counter--) {
            for (int column = CONTROL_SIGNALS_COLUMN;
                 column >= INITIAL_STATE_COLUMN;
                 column -= 2) {
                lineEditTo = (TTLineEdit*)getTtField(counter, column);
                lineEditFrom = (TTLineEdit*)getTtField(counter - 1, column);
                lineEditTo->setText(lineEditFrom->text());
                lineEditTo->setIsCorrect(lineEditFrom->getIsCorrect());
            }
        }
        for (int column = CONTROL_SIGNALS_COLUMN;
             column >= INITIAL_STATE_COLUMN;
             column -= 2) {
            lineEditTo = (TTLineEdit*)getTtField(lastMovingString - 1, column);
            lineEditTo->clear();
        }
        lineEditTo->setFocus();
    } else {
        QLabel *stringNumberLabel =
                new QLabel(QString::number(stringNumber + 1));
        stringNumberLabel->setMinimumWidth(20);
        stringNumberLabel->setAlignment(Qt::AlignCenter);
        stringNumberLabel->setStyleSheet(STRING_NUMBER_LABEL_STYLE);

        QSpacerItem *hSpacer1 = new QSpacerItem(10, 20,
                                                QSizePolicy::Fixed,
                                                QSizePolicy::Fixed);
        QSpacerItem *hSpacer2 = new QSpacerItem(8, 20,
                                                QSizePolicy::Fixed,
                                                QSizePolicy::Fixed);
        QSpacerItem *hSpacer3 = new QSpacerItem(8,20,
                                                QSizePolicy::Fixed,
                                                QSizePolicy::Fixed);
        QSpacerItem *hSpacer4 = new QSpacerItem(9, 20,
                                                QSizePolicy::Fixed,
                                                QSizePolicy::Fixed);

        QLineEdit *initialStateLineEdit =
                new TTLineEdit(this, stringNumber, 0);
        initialStateLineEdit->setMaximumWidth(86);
        initialStateLineEdit->setAlignment(Qt::AlignCenter);
        initialStateLineEdit->setMaxLength(3);
        initialStateLineEdit->setValidator(new StateValidator());
        connect(initialStateLineEdit, SIGNAL(editingFinished()),
                initialStateLineEdit, SLOT(onStateEditingFinished()));
        connect(initialStateLineEdit, SIGNAL(textChanged(const QString &)),
                initialStateLineEdit, SLOT(onStateChanged(const QString &)));

        QLineEdit *nextStateLineEdit =
                new TTLineEdit(this, stringNumber, 1);
        nextStateLineEdit->setMaximumWidth(86);
        nextStateLineEdit->setAlignment(Qt::AlignCenter);
        nextStateLineEdit->setMaxLength(3);
        nextStateLineEdit->setValidator(new StateValidator());
        connect(nextStateLineEdit, SIGNAL(editingFinished()),
                nextStateLineEdit, SLOT(onStateEditingFinished()));
        connect(nextStateLineEdit, SIGNAL(textChanged(const QString &)),
                nextStateLineEdit, SLOT(onStateChanged(const QString &)));

        QLineEdit *transitionConditionsLineEdit =
                new TTLineEdit(this, stringNumber, 2);
        transitionConditionsLineEdit->setMaximumWidth(235);
        transitionConditionsLineEdit->setTextMargins(5,0,5,0);
        transitionConditionsLineEdit->setMaxLength(27);
        connect(transitionConditionsLineEdit,
                SIGNAL(editingFinished()),
                transitionConditionsLineEdit,
                SLOT(onTransitionConditionsEditingFinished()));
        connect(transitionConditionsLineEdit,
                SIGNAL(textChanged(const QString &)),
                transitionConditionsLineEdit,
                SLOT(onTransitionConditionsChanged(const QString &)));

        QLineEdit *controlSignalsLineEdit =
                new TTLineEdit(this, stringNumber, 3);
        controlSignalsLineEdit->setTextMargins(5,0,5,0);
        controlSignalsLineEdit->setMaxLength(36);
        connect(controlSignalsLineEdit,
                SIGNAL(editingFinished()),
                controlSignalsLineEdit,
                SLOT(onControlSignalsEditingFinished()));
       connect(controlSignalsLineEdit,
               SIGNAL(textChanged(const QString &)),
               controlSignalsLineEdit,
               SLOT(onControlSignalsChanged(const QString &)));

        grid->addWidget(stringNumberLabel, stringNumber, 0);
        grid->addItem(hSpacer1, stringNumber, 1);
        grid->addWidget(initialStateLineEdit,
                        stringNumber, INITIAL_STATE_COLUMN);
        grid->addItem(hSpacer2, stringNumber, 3);
        grid->addWidget(nextStateLineEdit,
                        stringNumber, NEXT_STATE_COLUMN);
        grid->addItem(hSpacer3, stringNumber, 5);
        grid->addWidget(transitionConditionsLineEdit,
                        stringNumber, TRANSITION_CONDITIONS_COLUMN);
        grid->addItem(hSpacer4, stringNumber, 7);
        grid->addWidget(controlSignalsLineEdit,
                        stringNumber, CONTROL_SIGNALS_COLUMN);

        initialStateLineEdit->setFocus();

        QSignalMapper *signalMapper = new QSignalMapper(this) ;
        connect(initialStateLineEdit, SIGNAL(returnPressed()),
                signalMapper, SLOT(map()));
        connect(nextStateLineEdit, SIGNAL(returnPressed()),
                signalMapper, SLOT(map()));
        connect(transitionConditionsLineEdit, SIGNAL(returnPressed()),
                signalMapper, SLOT(map()));
        connect(controlSignalsLineEdit, SIGNAL(returnPressed()),
                signalMapper, SLOT(map()));

        signalMapper->setMapping(initialStateLineEdit,
                                 stringNumber + 1) ;
        signalMapper->setMapping(nextStateLineEdit,
                                 stringNumber + 1) ;
        signalMapper->setMapping(transitionConditionsLineEdit,
                                 stringNumber + 1) ;
        signalMapper->setMapping(controlSignalsLineEdit,
                                 stringNumber + 1) ;

        connect(signalMapper, SIGNAL(mapped(int)),
                this, SLOT(addString(int)));

        stringCounter++;
        isLastStringNeeded = false;
    }

    if (stringCounter == MAX_ROWS_ON_SCREEN + 1)  {
        ui->scrollArea->verticalScrollBar()->show();
    }
}

void MainWindow::deleteString() {
    if (rowInFocus == 0 && stringCounter == 1) {
        return;
    }

    TTLineEdit *lineEditTo;
    TTLineEdit *lineEditFrom;
    if (rowInFocus < stringCounter - 1) {
        for (int counter = rowInFocus;
             counter < stringCounter - 1;
             counter++) {
            for (int column = INITIAL_STATE_COLUMN;
                 column <= CONTROL_SIGNALS_COLUMN;
                 column += 2) {
                lineEditTo = (TTLineEdit*)getTtField(counter, column);
                lineEditFrom = (TTLineEdit*)getTtField(counter + 1, column);
                lineEditTo->setText(lineEditFrom->text());
                lineEditTo->setIsCorrect(lineEditFrom->getIsCorrect());
            }
        }
    }

    for (int column = INITIAL_STATE_COLUMN;
         column <= CONTROL_SIGNALS_COLUMN;
         column += 2) {
        lineEditTo = (TTLineEdit*)getTtField(stringCounter - 1, column);
        if (!lineEditTo->getIsCorrect()) {
            decrementNErrors();
        }
        delete lineEditTo;
    }
    delete getTtField(stringCounter - 1, 0);
    if (rowInFocus == stringCounter - 1) {
        rowInFocus--;
    }

    stringCounter--;

    if (stringCounter == MAX_ROWS_ON_SCREEN - 1) {
        ui->scrollArea->verticalScrollBar()->hide();
    }
}

QString MainWindow::appDataToText() {
    QString data;

    data += CONTROL_SEQUENCE;

    data += QString::number(stringCounter) + CONTROL_SYMBOL;
    for (int row = 0; row < stringCounter; row++) {
        for (int column = INITIAL_STATE_COLUMN;
             column <= CONTROL_SIGNALS_COLUMN;
             column += 2) {
            TTLineEdit *field = (TTLineEdit*)getTtField(row,column);
            data += field->text() + CONTROL_SYMBOL;
        }
    }

    for (int row = 0; row < RAM_ROWS_COUNT; row++) {
        for (int column = 0; column < RAM_COLUMNS_COUNT; column ++) {
            data += getRamCell(row, column)->text();
        }
    }

    return data;
}

void MainWindow::appDataFromText(QString data) {
    clearGrid();
    nErrors = 0;

    int pos = CONTROL_SEQUENCE.length();
    QChar c;

    QString nStringsText;
    while ((c = data[pos++]) != CONTROL_SYMBOL) {
        nStringsText += c;
    }
    for (stringCounter = 0; stringCounter < nStringsText.toInt();) {
        addString(stringCounter);
    }

    int row = 0;
    int column = INITIAL_STATE_COLUMN;
    while (row < stringCounter) {
        QString fieldText = "";
        while ((c = data[pos++]) != '\0' && c != CONTROL_SYMBOL) {
            fieldText += c;
        }

        TTLineEdit *field = (TTLineEdit*)getTtField(row,column);
        field->setText(fieldText);
        field->emitFocusOut();
        if (column < CONTROL_SIGNALS_COLUMN) {
            column += 2;
        } else {
            row++;
            column = 2;
        }
    }

    row = 0;
    column = 0;
    while(pos != data.length()) {
        QString cellText = "";
        for (int digitPos = 0; digitPos <= 1; digitPos++) {
            cellText += data[pos++];
        }
        getRamCell(row,column)->setText(cellText);
        if (column < RAM_COLUMNS_COUNT - 1) {
            column++;
        } else {
            row++;
            column = 0;
        }
    }
}


void MainWindow::rejectTtChangesAndLaunch() {
    ui->startButton->setEnabled(false);
    ui->addButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->scrollAreaContents->setEnabled(false);
    /*for (int row = 0; row < stringCounter; ++row) {
        for (int column = INITIAL_STATE_COLUMN;
             column <= CONTROL_SIGNALS_COLUMN;
             column += 2) {
            getTtField(row, column)->setEnabled(false);
        }
    }*/
}

void MainWindow::allowTtChangesAndLaunch() {
    ui->startButton->setEnabled(true);
    ui->addButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->scrollAreaContents->setEnabled(true);
    /*for (int row = 0; row < stringCounter; ++row) {
        for (int column = INITIAL_STATE_COLUMN;
             column <= CONTROL_SIGNALS_COLUMN;
             column += 2) {
            getTtField(row, column)->setEnabled(true);
        }
    }*/
}

void MainWindow::clearGrid() {
    for (int row = 0; row < stringCounter; row++) {
        for (int column = 0; column <= CONTROL_SIGNALS_COLUMN; column += 2) {
            delete getTtField(row,column);
        }
    }
}

QLineEdit* MainWindow::getTtField(int row, int column) {
    return (TTLineEdit*)grid->itemAtPosition(row,column)->widget();
}

RamLineEdit* MainWindow::getRamCell(int row, int column) {
    return (RamLineEdit*)ui->ramTable->cellWidget(row,column);
}

int MainWindow::getStringCounter() {
    return stringCounter;
}

void MainWindow::setFocus(int rowInFocus, int columnInFocus) {
    this->rowInFocus = rowInFocus;
    this->columnInFocus = columnInFocus;
}

int MainWindow::getRowInFocus() {
    return this->rowInFocus;
}

int MainWindow::getColumnInFocus() {
    return this->columnInFocus;
}

QTableWidget* MainWindow::getRamTable() {
    return ui->ramTable;
}

void MainWindow::incrementNErrors() {
    nErrors++;
}

void MainWindow::decrementNErrors() {
    nErrors--;
}
