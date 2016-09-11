#include "executionwindow.h"
#include "ui_executionwindow.h"
#include "mainwindow.h"
#include "errorsdialog.h"
#include "ttlineedit.h"
#include "ramlineedit.h"
#include "constants.h"
#include "styles.h"
#include "infodialog.h"

#include <QKeyEvent>
#include <QDialog>

ExecutionWindow::ExecutionWindow(MainWindow *mainWindow, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ExecutionWindow) {
    ui->setupUi(this);
    this->mainWindow = mainWindow;
    connect(mainWindow, SIGNAL(closed()), this, SLOT(close()));
    ui->nextButton->setFocus();

    isProgramFinished = false;
    isProgramStopped = false;
    nextTransitionExists = true;
    areConditionsDescribed = false;
    isProgramOnRewindToEnd = false;
    step = 1;
    initialState = 0;
    nextState = 0;
    conditionsText = "";
    signalsText = "";
    operationString = "";
    RK = new int[RK_SIZE]();
    RON = new int[RON_NUMBER]();
    A = 0;
    B = 0;
    RS = 0;
    SP = 0;
    RI = 0;
    RA = 0;
    RV = 0;
    R = 0;
    S1 = 0;
    S2 = 0;
    S3 = 0;
    X4 = 0;
    X3 = 0;
    X2 = 0;
    X1 = 0;
    Z = 0;
    N = 0;
    C = 0;
    P = 0;
    signalsFlags = new int[MAX_SIGNAL_FLAG + 1]();
    currentSignalNumber = -1;

    moveToNextState();
    executeNextOperation();
}

ExecutionWindow::~ExecutionWindow() {
    delete [] RK;
    delete [] RON;
    delete [] signalsFlags;
    delete ui;
}

void ExecutionWindow::closeEvent(QCloseEvent *event) {
    this->deleteLater();
}

void ExecutionWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        this->deleteLater();
    } else {
        QDialog::keyPressEvent(event);
    }
}

void ExecutionWindow::on_nextButton_pressed() {
    ui->nextButton->move(ui->nextButton->x() - 1,
                         ui->nextButton->y() - 1);
    ui->nextButton->setIconSize(*(new QSize(
                                      START_NEXT_PRESSED_SIZE,
                                      START_NEXT_PRESSED_SIZE
                                      )));
}

void ExecutionWindow::on_nextButton_clicked() {
    ui->nextButton->move(ui->nextButton->x() + 1,
                         ui->nextButton->y() + 1);
    ui->nextButton->setIconSize(*(new QSize(
                                      START_NEXT_UNPRESSED_SIZE,
                                      START_NEXT_UNPRESSED_SIZE
                                      )));
    executeNextOperation();
}

void ExecutionWindow::on_endButton_pressed() {
    ui->endButton->move(ui->endButton->x() - 1,
                        ui->endButton->y() - 1);
    ui->endButton->setIconSize(*(new QSize(
                                     ui->endButton->iconSize().width() - 1,
                                     ui->endButton->iconSize().height() - 1
                                     )));
}

void ExecutionWindow::on_endButton_clicked() {
    ui->endButton->move(ui->endButton->x() + 1,
                        ui->endButton->y() + 1);
    ui->endButton->setIconSize(*(new QSize(
                                     ui->endButton->iconSize().width() + 1,
                                     ui->endButton->iconSize().height() + 1
                                     )));

    isProgramOnRewindToEnd = true;
    while (nextTransitionExists && step < MAX_REWIND_STEP) {
        executeNextOperation();
    }
    if (step >= MAX_REWIND_STEP) {
        QString error = tr("Перемотка остановлена");
        QString info = tr("Ваша программа выполняется циклически\n") ;
        info += tr("или содержит слишком большое количество тактов.\n");
        info += tr("Воспользуйте пошаговым выполнением.");
        ErrorsDialog *errorsDialog = new ErrorsDialog(error, info);
        (*errorsDialog).exec();
        delete errorsDialog;
    }
    ui->nextButton->setFocus();
}

void ExecutionWindow::moveToNextState() {
    areConditionsDescribed = false;
    moveToNextState(0);
}

void ExecutionWindow::moveToNextState(int firstRow) {
    for (int i = 0; i <= MAX_SIGNAL_FLAG; i++) {
        signalsFlags[i] = 0;
    }

    int requiredInitialState = nextState;
    row = firstRow - 1;
    initialState = -1;
    while (row != mainWindow->getStringCounter() - 1
           && initialState != requiredInitialState) {
        row++;
        QString initialStateText =
                ((TTLineEdit*)mainWindow->getTtField(row,INITIAL_STATE_COLUMN))
                ->text();
        if (!initialStateText.isEmpty()) {
            initialState = initialStateText.toInt();
        }
    }

    if (row == mainWindow->getStringCounter() - 1
            && (initialState != requiredInitialState
                || !areConditionsSatisfied())) {
        nextTransitionExists = false;
        return;
    }

    if (firstRow == 0) {
        TTLineEdit *controlSignalsLineEdit =
                (TTLineEdit*)mainWindow
                ->getTtField(row, CONTROL_SIGNALS_COLUMN);
        signalsText = controlSignalsLineEdit->text();
        controlSignals = controlSignalsLineEdit->getControlSignals();
        if (!signalsText.isEmpty()) {
            currentSignalNumber = 0;
            while (controlSignals[currentSignalNumber] != 0
                   && controlSignals[currentSignalNumber] <= MAX_SIGNAL_FLAG) {
                signalsFlags[controlSignals[currentSignalNumber++]] = true;
            }

            if (controlSignals[currentSignalNumber] == 0) {
                currentSignalNumber = -1;
            }
        } else {
            currentSignalNumber = -1;
        }
    }
    if (!areConditionsSatisfied()) {
        if (firstRow != 0) {
            moveToNextState(row + 1);
        }
    } else {
        nextState = ((TTLineEdit*)mainWindow->getTtField(row, NEXT_STATE_COLUMN))
                    ->text().toInt();
    }
}

void ExecutionWindow::executeNextOperation() {
    if (isProgramFinished) {
        this->deleteLater();
    }

    if (!nextTransitionExists) {
        printValues("before");
        hideValues();
        QString nonTransitionString = tr("Не описан переход из состояния  ");
        nonTransitionString += QString::number(nextState);
        if (areConditionsDescribed) {
            nonTransitionString += tr("  при данных условиях");
        }
        QString programStoppedString = tr("Успешное завершение программы");
        if (isProgramStopped) {
            ui->operationLabel->setText(programStoppedString);
        } else {
            ui->operationLabel->setText(nonTransitionString);
        }
        ui->operationLabel->move(ui->operationLabel->x(),
                                 ui->operationLabel->y() - 120);
        ui->beforeGroupBox->show();
        ui->beforeGroupBox->move(ui->beforeGroupBox->x(),
                                 ui->beforeGroupBox->y() + 70);
        isProgramFinished = true;
        return;
    }

    if (currentSignalNumber == -1) {
        operationString = tr("Нет микроопераций");
        printValues("after");
        if (areConditionsSatisfied()) {
            moveToNextState();
            step++;
        } else {
            nextState = initialState;
            areConditionsDescribed = true;
            moveToNextState(row + 1);
            executeNextOperation();
        }
        return;
    }
    if (controlSignals[currentSignalNumber] == 0) {
        if (areConditionsSatisfied()) {
            moveToNextState();
        } else {
            nextState = initialState;
            areConditionsDescribed = true;
            moveToNextState(row + 1);
        }
        executeNextOperation();
        return;
    }
    step++;
    printValues("before");
    Z = 0;
    N = 0;
    C = 0;
    P = 0;
    operationString = tr("Выполнена микрооперация y");
    operationString += QString::number(controlSignals[currentSignalNumber]);
    operationString += tr(" :    ");

    int T;
    int result = 1;
    int R1 = S1 / HEX;
    int R2 = S2 % HEX;
    switch (controlSignals[currentSignalNumber]) {
    case 20:
        operationString += tr("A := R");
        A = R;
        result = A;
        break;
    case 21:
        operationString += tr("A := S1");
        A = S1;
        result = A;
        break;
    case 22:
        operationString += tr("A := S2");
        A = S2;
        result = A;
        break;
    case 23:
        operationString += tr("A := S3");
        A = S3;
        result = A;
        break;
    case 24:
        operationString += tr("A := PC");
        A = RS;
        result = A;
        break;
    case 25:
        operationString += tr("A := PB");
        A = RV;
        result = A;
        break;
    case 26:
        operationString += tr("A := SP");
        A = SP;
        result = A;
        break;
    case 27:
        operationString += tr("A := POH[R1]");
        A = RON[R1];
        result = A;
        break;
    case 28:
        operationString += tr("A := POH[R2]");
        A = RON[R2];
        result = A;
        break;
    case 29:
        operationString += tr("A := POH[T]");
        T = calculateT();
        A = RON[T];
        result = A;
        break;
    case 30:
        operationString += tr("A := ОП[PA]");
        A = readFromRam(RA);
        result = A;
        break;
    case 31:
        operationString += tr("A := ");
        A = executeAluOperation(A, "A", B, "B");
        result = A;
        break;
    case 35:
        operationString += tr("B := РИ");
        B = RI;
        result = B;
        break;
    case 36:
        operationString += tr("B := POH[R]");
        B = RON[R];
        result = B;
        break;
    case 37:
        operationString += tr("B := ");
        B = executeAluOperation(A, "A", B, "B");
        result = B;
        break;
    case 38:
        operationString += tr("B := F.R");
        B = MAX_UNSIGNED_HEX_VALUE - HEX + 1 + R;
        result = B;
        break;
    case 41:
        operationString += tr("PA := POH[T]");
        T = calculateT();
        RA = RON[T];
        result = RA;
        break;
    case 42:
        operationString += tr("PA := ");
        RA = executeAluOperation(A, "A", B, "B");
        result = RA;
        break;
    case 43:
        operationString += tr("PA := 0");
        RA = 0;
        result = RA;
        break;
    case 45:
        operationString += tr("PB := A");
        RV = A;
        result = RV;
        break;
    case 47:
        operationString += tr("РИ := РИ ");
        if (!signalsFlags[6]) {
            operationString += "+ 1";
            checkCarryAndOverflow(RI, 1);
            ++RI;
            if (RI > MAX_UNSIGNED_HEX_VALUE) {
                RI -= MAX_UNSIGNED_HEX_VALUE + 1;
            }
        } else {
            operationString += "- 1";
            checkCarryAndOverflow(RI, 1, true);
            --RI;
            if (RI < 0) {
                RI += MAX_UNSIGNED_HEX_VALUE + 1;
            }
        }
        result = RI;
        break;
    case 48:
        operationString += tr("РИ := A");
        RI = A;
        result = RI;
        break;
    case 49:
        operationString += tr("PC := 0");
        RS = 0;
        result = RS;
        break;
    case 50:
        operationString += tr("PC := ");
        RS = executeAluOperation(RS, "PC", A, "A");
        result = RS;
        break;
    case 52:
        operationString += tr("SP := A");
        SP = A;
        result = SP;
        break;
    case 53:
        operationString += tr("SP := ");
        SP = executeAluOperation(SP, "SP", A, "A");
        result = SP;
        break;
    case 55:
        operationString += tr("POH[R] := S1");
        RON[R] = S1;
        result = RON[R];
        break;
    case 56:
        operationString += tr("POH[R1] := A");
        RON[R1] = A;
        result = RON[R1];
        break;
    case 57:
        operationString += tr("POH[R] := ");
        RON[R] = executeAluOperation(RON[R], "POH[R]", A, "A");
        result = RON[R];
        break;
    case 58:
        operationString += tr("POH[T] := ");
        T = calculateT();
        RON[T] = executeAluOperation(RON[T], "POH[T]", A, "A");
        result = RON[T];
        break;
    case 60:
        operationString += tr("ОП[РА] := A");
        writeToRam();
        result = A;
        break;
    case 62:
        operationString += tr("PK[31:16] := ОП[РА]");
        readRk(1);
        break;
    case 63:
        operationString += tr("PK[31:8] := ОП[РА]");
        readRk(2);
        break;
    case 64:
        operationString += tr("PK[31:0] := ОП[РА]");
        readRk(3);
        break;
    case 66:
        operationString += tr("A := Дисплей");
        if (!isProgramOnRewindToEnd) {
            QString infoString = tr("Введите байт данных: ");
            InfoDialog *infoDialog = new InfoDialog(this, infoString, true);
            (*infoDialog).exec();
            delete infoDialog;
        }
        break;
    case 67:
        operationString += tr("Дисплей := A");
        if (!isProgramOnRewindToEnd) {
            QString infoString = "A = " + toHexString(A);
            InfoDialog *infoDialog = new InfoDialog(this, infoString);
            (*infoDialog).exec();
            delete infoDialog;
        }
        break;
    case 68:
        operationString += tr("\"Переполнение\"");
        if (!isProgramOnRewindToEnd) {
            QString infoString = tr("Переполнение");
            InfoDialog *infoDialog = new InfoDialog(this, infoString);
            (*infoDialog).exec();
            delete infoDialog;
        }
        break;
    case 69:
        operationString += tr("\"Ошибка в КОП\"");
        if (!isProgramOnRewindToEnd) {
            QString infoString = tr("Ошибка в коде операции");
            InfoDialog *infoDialog = new InfoDialog(this, infoString);
            (*infoDialog).exec();
            delete infoDialog;
        }
        break;
    case 70:
        operationString += tr("\"СТОП\"");
        if (!isProgramOnRewindToEnd) {
            QString infoString = tr("СТОП");
            InfoDialog *infoDialog = new InfoDialog(this, infoString);
            (*infoDialog).exec();
            delete infoDialog;
        }
        nextTransitionExists = false;
        isProgramStopped = true;
        break;


    default:
        result = 1;
        break;
    }

    if (result == 0) {
        Z = true;
    }
    if (result > MAX_SIGNED_HEX_VALUE) {
        N = true;
    }
    printValues("after");

    currentSignalNumber++;
}

int ExecutionWindow::executeAluOperation(int x, QString xName,
                                         int y, QString yName) {
    bool y11 = signalsFlags[11];
    bool y12 = signalsFlags[12];
    bool y13 = signalsFlags[13];
    bool y14 = signalsFlags[14];
    bool y15 = signalsFlags[15];
    QString operationBinString = QString::number(y11) + QString::number(y12)
                                 + QString::number(y13) + QString::number(y14);
    int aluOperation = operationBinString.toInt(0, BIN);
    if (y15) {
        switch (aluOperation) {
        case 0:
            operationString += "!" + xName;
            return bitwiseNot(x);
        case 1:
            operationString += "!(" + xName + " v " + yName + ")";
            return bitwiseNot(x | y);
        case 2:
            operationString += "!" + xName + " & " + yName;
            return bitwiseNot(x) & y;
        case 3:
            operationString += "0";
            return 0;
        case 4:
            operationString += "!(" + xName + " & " + yName + ")";
            return bitwiseNot(x & y);
        case 5:
            operationString += "!" + yName;
            return bitwiseNot(y);
        case 6:
            operationString += xName + " ^ " + yName;
            return x ^ y;
        case 7:
            operationString += xName + " & !" + yName;
            return x & bitwiseNot(y);
        case 8:
            operationString += "!" + xName + " v " + yName;
            return bitwiseNot(x) | y;
        case 9:
            operationString += "!(" + xName + " ^ " + yName + ")";
            return bitwiseNot(x ^ y);
        case 10:
            operationString += yName;
            return y;
        case 11:
            operationString += xName + " & " + yName;
            return x & y;
        case 12:
            operationString += "FF";
            return MAX_UNSIGNED_HEX_VALUE;
        case 13:
            operationString += xName + " v !" + yName;
            return x | bitwiseNot(y);
        case 14:
            operationString += xName + " v " + yName;
            return x | y;
        case 15:
            operationString += xName;
            return x;
        default:
            return IMPOSSIBLE_VALUE;
        }
    } else {
        int result = IMPOSSIBLE_VALUE;
        switch (aluOperation) {
        case 0:
            operationString += xName + " + 1";
            result = x + 1;
            checkCarryAndOverflow(x, 1);
            break;
        case 1:
            operationString += xName + " << 1 (1)";
            result = bitwiseLeftShift(x, 1);
            break;
        case 2:
            operationString += xName + " + 2";
            result = x + 2;
            checkCarryAndOverflow(x, 2);
            break;
        case 4:
            operationString += xName + " >> 1 (0)";
            result = bitwiseRightShift(x, 0);
            break;
        case 5:
            operationString += xName + " + 3";
            result = x + 3;
            checkCarryAndOverflow(x, 3);
            break;
        case 6:
            operationString += xName + " - " + yName;
            result = x - y;
            checkCarryAndOverflow(x, y, true);
            break;
        case 8:
            operationString += xName + " >> 1 (1)";
            result = bitwiseRightShift(x, 1);
            break;
        case 9:
            operationString += xName + " + " + yName;
            result =  x + y;
            checkCarryAndOverflow(x, y);
            break;
        case 10:
            operationString += yName + " - " + xName;
            result =  y - x;
            checkCarryAndOverflow(y, x, true);
            break;
        case 11:
            operationString += xName + " + 4";
            result = x + 4;
            checkCarryAndOverflow(x, 4);
            break;
        case 12:
            operationString += xName + " << 1 (0)";
            result = bitwiseLeftShift(x, 0);
            break;
        case 15:
            operationString += xName + " - 1";
            result = x - 1;
            checkCarryAndOverflow(x, 1, true);
            break;
        default:
            break;
        }

        if (result > MAX_UNSIGNED_HEX_VALUE) {
            result -= MAX_UNSIGNED_HEX_VALUE + 1;
        } else if (result < 0) {
            result += MAX_UNSIGNED_HEX_VALUE + 1;
        }

        return result;
    }
}

bool ExecutionWindow::areConditionsSatisfied() {
    TTLineEdit *transitionConditionsLineEdit =
            (TTLineEdit*)mainWindow
            ->getTtField(row,TRANSITION_CONDITIONS_COLUMN);
    conditionsText = transitionConditionsLineEdit->text();

    if (conditionsText.isEmpty() || conditionsText == "b") {
        return true;
    }
    if (conditionsText == "-b" || conditionsText == "!b") {
        return false;
    }

    int *conditionsNumbers = transitionConditionsLineEdit
                             ->getConditionsNumbers();
    bool *conditionsSigns = transitionConditionsLineEdit
                            ->getConditionsSigns();
    int pos = 0;
    while (conditionsNumbers[pos] != -1) {
        switch(conditionsNumbers[pos]) {
        case 0:
            if (X4 == conditionsSigns[pos]) {
                break;
            } else {
                return false;
            }
        case 1:
            if (X3 == conditionsSigns[pos]) {
                break;
            } else {
                return false;
            }
        case 2:
            if (X2 == conditionsSigns[pos]) {
                break;
            } else {
                return false;
            }
        case 3:
            if (X1 == conditionsSigns[pos]) {
                break;
            } else {
                return false;
            }
        case 4:
            if (Z == conditionsSigns[pos]) {
                break;
            } else {
                return false;
            }
        case 5:
            if (N == conditionsSigns[pos]) {
                break;
            } else {
                return false;
            }
        case 6:
            if (C == conditionsSigns[pos]) {
                break;
            } else {
                return false;
            }
        case 7:
            if (P == conditionsSigns[pos]) {
                break;
            } else {
                return false;
            }
        default:
            break;
        }
        pos++;
    }
    return true;
}

int ExecutionWindow::calculateT() {
    bool y7 = signalsFlags[7];
    bool y8 = signalsFlags[8];
    bool y9 = signalsFlags[9];
    bool y10 = signalsFlags[10];
    QString tBinString = QString::number(y10) + QString::number(y9)
                        + QString::number(y8) + QString::number(y7);
    int T = tBinString.toInt(0,BIN);
    return T;
}

int ExecutionWindow::bitwiseNot(int x) {
    bool *binX = toBinArray(x);
    for (int i = 0; i < BIN_LENGTH; i++) {
        binX[i] = !binX[i];
    }
    return fromBinArray(binX);
}

int ExecutionWindow::bitwiseLeftShift(int x, bool valueToInsert) {
    bool *binX = toBinArray(x);
    if (binX[0]) {
        C = true;
    }
    if (binX[0] != binX[1]) {
        P = true;
    }

    for (int i = 0; i < BIN_LENGTH - 1; ++i) {
        binX[i] = binX[i + 1];
    }
    binX[BIN_LENGTH - 1] = valueToInsert;

    return fromBinArray(binX);
}

int ExecutionWindow::bitwiseRightShift(int x, bool valueToInsert) {
    bool *binX = toBinArray(x);
    if (binX[BIN_LENGTH - 1]) {
        C = true;
    }
    if (binX[0] != valueToInsert) {
        P = true;
    }

    for (int i = BIN_LENGTH - 1; i > 0; --i) {
        binX[i] = binX[i - 1];
    }
    binX[0] = valueToInsert;

    return fromBinArray(binX);
}

void ExecutionWindow::readRk(int commandAdressesNumber) {
    if (RA + commandAdressesNumber <= MAX_UNSIGNED_HEX_VALUE) {
        for (int i = 0; i <= commandAdressesNumber; i++) {
            RK[i] = readFromRam(RA + i);
        }
        for (int i = commandAdressesNumber + 1; i < RK_SIZE; i++) {
            RK[i] = 0;
        }

        R = RK[0] % HEX;
        S1 = RK[1];
        S2 = RK[2];
        S3 = RK[3];

        int xConditions = RK[0] / HEX;
        bool *binXConditions = toBinArray(xConditions);
        X4 = binXConditions[4];
        X3 = binXConditions[5];
        X2 = binXConditions[6];
        X1 = binXConditions[7];
    } else {
        operationString = tr("<font color='red'>");
        operationString += tr("Невозможно выполнить микрооперацию ");
        switch (commandAdressesNumber) {
        case 1:
            operationString += tr("y62: PK[31:16]");
            break;
        case 2:
            operationString += tr("y63: PK[31:8]");
            break;
        case 3:
            operationString += tr("y64: PK[31:0]");
            break;
        default:
            break;
        }
        operationString += tr(" := ОП[PA]</font>");
    }
}

int ExecutionWindow::readFromRam(int address) {
    QTableWidget *ramTable = mainWindow->getRamTable();
    int row = address / HEX;
    int column = address % HEX;
    QString hexString = ((QLineEdit*)ramTable->cellWidget(row,column))->text();
    return hexString.toInt(0,HEX);
}

void ExecutionWindow::writeToRam() {
    QString aString = toHexString(A);
    int row = RA / HEX;
    int column = RA % HEX;
    ((QLineEdit*)mainWindow->getRamTable()
            ->cellWidget(row, column))
            ->setText(aString);
}

void ExecutionWindow::checkCarryAndOverflow(int x, int y,
                                            bool isOperationSubstraction) {
    bool *binX = toBinArray(x);
    bool *binY = toBinArray(y);
    bool *carries = new bool[BIN_LENGTH + 1]();
    if (!isOperationSubstraction) {
        for (int i = BIN_LENGTH - 1; i >= 0; --i) {
            if ((binX[i] && binY[i])
                    || (carries[i + 1] && binX[i])
                    || (carries[i + 1] && binY[i])) {
                carries[i] = true;
            }
        }
    } else {
        for (int i = BIN_LENGTH - 1; i >= 0; --i) {
            if ((!binX[i] && binY[i])
                    || (carries[i + 1] && binY[i])
                    || (carries[i + 1] && !binX[i] && !binY[i])) {
                carries[i] = true;
            }
        }
    }

    if (carries[0] && carries[1]) {
        C = 1;
        P = 0;
    } else if (carries[0] && !carries[1]) {
        C = 1;
        P = 1;
    } else if (!carries[0] && carries[1]) {
        C = 0;
        P = 1;
    } else {
        C = 0;
        P = 0;
    }

    delete binX;
    delete binY;
    delete [] carries;
}

void ExecutionWindow::printValues(QString location) {
    if (location == "before") {
        ui->rk1BeforeLabel->setText(toHexString(RK[0]));
        ui->rk2BeforeLabel->setText(toHexString(RK[1]));
        ui->rk3BeforeLabel->setText(toHexString(RK[2]));
        ui->rk4BeforeLabel->setText(toHexString(RK[3]));
        ui->ron0BeforeLabel->setText(toHexString(RON[0]));
        ui->ron1BeforeLabel->setText(toHexString(RON[1]));
        ui->ron2BeforeLabel->setText(toHexString(RON[2]));
        ui->ron3BeforeLabel->setText(toHexString(RON[3]));
        ui->ron4BeforeLabel->setText(toHexString(RON[4]));
        ui->ron5BeforeLabel->setText(toHexString(RON[5]));
        ui->ron6BeforeLabel->setText(toHexString(RON[6]));
        ui->ron7BeforeLabel->setText(toHexString(RON[7]));
        ui->ron8BeforeLabel->setText(toHexString(RON[8]));
        ui->ron9BeforeLabel->setText(toHexString(RON[9]));
        ui->ron10BeforeLabel->setText(toHexString(RON[10]));
        ui->ron11BeforeLabel->setText(toHexString(RON[11]));
        ui->ron12BeforeLabel->setText(toHexString(RON[12]));
        ui->ron13BeforeLabel->setText(toHexString(RON[13]));
        ui->ron14BeforeLabel->setText(toHexString(RON[14]));
        ui->ron15BeforeLabel->setText(toHexString(RON[15]));
        ui->aBeforeLabel->setText(toHexString(A));
        ui->bBeforeLabel->setText(toHexString(B));
        ui->rsBeforeLabel->setText(toHexString(RS));
        ui->spBeforeLabel->setText(toHexString(SP));
        ui->riBeforeLabel->setText(toHexString(RI));
        ui->raBeforeLabel->setText(toHexString(RA));
        ui->rvBeforeLabel->setText(toHexString(RV));
        ui->rBeforeLabel->setText(toHexString(R));
        ui->s1BeforeLabel->setText(toHexString(S1));
        ui->s2BeforeLabel->setText(toHexString(S2));
        ui->s3BeforeLabel->setText(toHexString(S3));
        ui->x4BeforeLabel->setText(QString::number(X4));
        ui->x3BeforeLabel->setText(QString::number(X3));
        ui->x2BeforeLabel->setText(QString::number(X2));
        ui->x1BeforeLabel->setText(QString::number(X1));
        ui->zBeforeLabel->setText(QString::number(Z));
        ui->nBeforeLabel->setText(QString::number(N));
        ui->cBeforeLabel->setText(QString::number(C));
        ui->pBeforeLabel->setText(QString::number(P));
    } else if (location == "after") {
        ui->rowLabel->setText(QString::number(row + 1));
        ui->stepLabel->setText(QString::number(step));
        ui->initialStateLabel->setText(QString::number(initialState));
        if (conditionsText.isEmpty()) {
            ui->nextStateLabel->setText(QString::number(nextState));
        } else {
            ui->nextStateLabel->setText(tr("По условию"));
        }
        ui->controlSignalsLabel->setText(signalsText);
        ui->operationLabel->setText(operationString);
        ui->rk1AfterLabel->setText(toHexString(RK[0]));
        ui->rk2AfterLabel->setText(toHexString(RK[1]));
        ui->rk3AfterLabel->setText(toHexString(RK[2]));
        ui->rk4AfterLabel->setText(toHexString(RK[3]));
        ui->ron0AfterLabel->setText(toHexString(RON[0]));
        ui->ron1AfterLabel->setText(toHexString(RON[1]));
        ui->ron2AfterLabel->setText(toHexString(RON[2]));
        ui->ron3AfterLabel->setText(toHexString(RON[3]));
        ui->ron4AfterLabel->setText(toHexString(RON[4]));
        ui->ron5AfterLabel->setText(toHexString(RON[5]));
        ui->ron6AfterLabel->setText(toHexString(RON[6]));
        ui->ron7AfterLabel->setText(toHexString(RON[7]));
        ui->ron8AfterLabel->setText(toHexString(RON[8]));
        ui->ron9AfterLabel->setText(toHexString(RON[9]));
        ui->ron10AfterLabel->setText(toHexString(RON[10]));
        ui->ron11AfterLabel->setText(toHexString(RON[11]));
        ui->ron12AfterLabel->setText(toHexString(RON[12]));
        ui->ron13AfterLabel->setText(toHexString(RON[13]));
        ui->ron14AfterLabel->setText(toHexString(RON[14]));
        ui->ron15AfterLabel->setText(toHexString(RON[15]));
        ui->aAfterLabel->setText(toHexString(A));
        ui->bAfterLabel->setText(toHexString(B));
        ui->rsAfterLabel->setText(toHexString(RS));
        ui->spAfterLabel->setText(toHexString(SP));
        ui->riAfterLabel->setText(toHexString(RI));
        ui->raAfterLabel->setText(toHexString(RA));
        ui->rvAfterLabel->setText(toHexString(RV));
        ui->rAfterLabel->setText(toHexString(R));
        ui->s1AfterLabel->setText(toHexString(S1));
        ui->s2AfterLabel->setText(toHexString(S2));
        ui->s3AfterLabel->setText(toHexString(S3));
        ui->x4AfterLabel->setText(QString::number(X4));
        ui->x3AfterLabel->setText(QString::number(X3));
        ui->x2AfterLabel->setText(QString::number(X2));
        ui->x1AfterLabel->setText(QString::number(X1));
        ui->zAfterLabel->setText(QString::number(Z));
        ui->nAfterLabel->setText(QString::number(N));
        ui->cAfterLabel->setText(QString::number(C));
        ui->pAfterLabel->setText(QString::number(P));
    } else {
    }
}

void ExecutionWindow::hideValues() {
    ui->label->hide();
    ui->rowLabel->hide();
    ui->label_8->hide();
    ui->stepLabel->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_7->hide();
    ui->initialStateLabel->hide();
    ui->nextStateLabel->hide();
    ui->controlSignalsLabel->hide();
    ui->beforeGroupBox->hide();
    ui->afterGroupBox->hide();
    ui->label_4->hide();
    ui->label_6->hide();
}

QString ExecutionWindow::toHexString(int number) {
    QString hexString = QString::number(number, HEX).toUpper();
    if (hexString.length() == 1) {
        hexString.insert(0,"0");
    }
    return hexString;
}

bool* ExecutionWindow::toBinArray(int number) {
    QString binString = QString::number(number, BIN);
    bool *binArray = new bool[BIN_LENGTH]();
    int stringPos;
    int arrayPos;
    for (stringPos = 0, arrayPos = BIN_LENGTH - binString.length();
         stringPos < binString.length();
         stringPos++, arrayPos++) {
        if (binString[stringPos] == '1') binArray[arrayPos] = 1;
    }
    return binArray;
}

int ExecutionWindow::fromBinArray(bool *binArray) {
    QString binString = "";
    for (int i = 0; i < BIN_LENGTH; i++) {
        binString += QString::number(binArray[i]);
    }
    return binString.toInt(0, BIN);
}

void ExecutionWindow::setA(int A) {
    this->A = A;
}
