#include "mainwindow.h"
#include "ttlineedit.h"
#include "styles.h"
#include "constants.h"
#include "QtDebug"

TTLineEdit::TTLineEdit(MainWindow *mainWindow, int row, int column) :
        QLineEdit() {
    this->mainWindow = mainWindow;
    this->row = row;
    this->column = (column + 1) * 2;
    this->setStyleSheet(TT_LINE_EDIT_STYLE);
    this->isCorrect = true;
}

TTLineEdit::~TTLineEdit() {
}

void TTLineEdit::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_W || event->key() == Qt::Key_Up) {
        if (row > 0) {
            mainWindow->getTtField(row - 1, column)->setFocus();
        }
    }
    else if(event->key() == Qt::Key_S || event->key() == Qt::Key_Down) {
        if (row < mainWindow->getStringCounter() - 1) {
            mainWindow->getTtField(row + 1, column)->setFocus();
        }
    }
    else if(event->key() == Qt::Key_A
            || (event->key() == Qt::Key_Left && this->cursorPosition() == 0)) {
        if (column > INITIAL_STATE_COLUMN) {
            mainWindow->getTtField(row, column - 2)->setFocus();
        } else {
            if (row > 0) {
                QLineEdit *previousField =
                        mainWindow->getTtField(row - 1, CONTROL_SIGNALS_COLUMN);
                int previousFieldLength = previousField->text().length();
                previousField->setFocus();
                previousField->setCursorPosition(previousFieldLength);
            }
        }
    }
    else if(event->key() == Qt::Key_D
            || (event->key() == Qt::Key_Right
                && this->cursorPosition() == this->text().length())) {
        if (column < CONTROL_SIGNALS_COLUMN) {
            mainWindow->getTtField(row, column + 2)->setFocus();
        } else {
            if (row < mainWindow->getStringCounter() - 1) {
                QLineEdit *nextField =
                        mainWindow->getTtField(row + 1, INITIAL_STATE_COLUMN);
               nextField->setFocus();
               nextField->setCursorPosition(0);
            }
        }
    }
    else if(event->key() == Qt::Key_Backspace
            && this->cursorPosition() == 0
            && this->selectedText().isEmpty()) {
        if (mainWindow->getColumnInFocus() == INITIAL_STATE_COLUMN) {
            mainWindow->deleteString();
            if (row != 0) {
                mainWindow->getTtField(row - 1, column)->setFocus();
            }
        } else {
            mainWindow->getTtField(row, column - 2)->setFocus();
        }
    }
    else {
        QLineEdit::keyPressEvent(event);
    }
}

void TTLineEdit::focusInEvent(QFocusEvent *event) {
    if (column == INITIAL_STATE_COLUMN
            || column == NEXT_STATE_COLUMN) {
        this->setIsCorrect(true);
    }
    mainWindow->setFocus(row,column);
    QLineEdit::focusInEvent(event);
}

void TTLineEdit::emitFocusOut() {
    emit(this->focusOutEvent(new QFocusEvent(QFocusEvent::FocusOut)));
}

bool TTLineEdit::areTransitionConditionsCorrect() {
    const QString data = this->text().toLower();

    if (data.isEmpty() || data == "b" || data == "-b" || data == "!b") {
        return true;
    }

    bool *isAlreadyUsed = new bool[POSSIBLE_CONDITIONS_COUNT + 1]();
    conditionsSignsSymbols = new QChar[MAX_CONDITIONS_COUNT + 1]();
    conditionsNumbers = new int[MAX_CONDITIONS_COUNT + 1];
    for (int i = 0; i < MAX_CONDITIONS_COUNT + 1; i++) {
        conditionsNumbers[i] = -1;
    }
    int conditionsCounter = 0;

    bool isDataWrong = false;
    QChar c;
    int commonCounter = 0;
    bool isDataEnded = false;
    isDataWrong = false;
    while (!isDataEnded && !isDataWrong) {
        QString currentCondition = "";

        while (commonCounter != data.length() - 1
               && data[commonCounter] == ' ') commonCounter++;
        if ( commonCounter != data.length() - 1
             && (data[commonCounter] == '-' || data[commonCounter] == '!')) {
                conditionsSignsSymbols[conditionsCounter] = data[commonCounter];
                commonCounter++;
        }

        do {
            c = data[commonCounter++];
            if (c != ' ' && c != ',') {
                currentCondition += c;
            }

            if (commonCounter == data.length()) {
                isDataEnded = true;
                break;
            }
        } while (c != ',');

        if (currentCondition != "") {
            isDataWrong = true;
            for (int i = 0; i < POSSIBLE_CONDITIONS_COUNT; i++) {
                if (currentCondition == POSSIBLE_CONDITIONS[i]
                        && isAlreadyUsed[i] == false) {
                    isDataWrong = false;
                    conditionsNumbers[conditionsCounter++] = i;
                    isAlreadyUsed[i] = true;
                }
            }
        }
    }

    if (isDataWrong) {
        return false;
    } else {
        return true;
    }
}

void TTLineEdit::sortTransitionConditions() {
    QString conditionsString = this->text();
    if (conditionsString.isEmpty()
            || conditionsString == "b"
            || conditionsString == "-b"
            || conditionsString == "!b") {
        return;
    }

    insertionSort(conditionsNumbers, conditionsSignsSymbols, -1);
    conditionsSigns = new bool[MAX_CONDITIONS_COUNT + 1]();
    for (int i = 0; i < MAX_CONDITIONS_COUNT + 1; i++) {
        if (conditionsSignsSymbols[i] == 0) {
            conditionsSigns[i] = true;
        }
    }

    conditionsString = "";
    int index = 0;
    while (conditionsNumbers[index] != -1) {
        if (conditionsSignsSymbols[index] == '-'
                || conditionsSignsSymbols[index] == '!') {
            conditionsString += conditionsSignsSymbols[index];
        }
        int conditionNumber = conditionsNumbers[index];
        conditionsString += POSSIBLE_CONDITIONS[conditionNumber];
        if (index <= MAX_CONDITIONS_COUNT - 2
                && conditionsNumbers[index + 1] != -1) {
            conditionsString += ',';
        }
        index++;
    }
    this->setText(conditionsString);
}

bool TTLineEdit::areControlSignalsCorrect() {
    const QString data = this->text().toLower();
    QString *controlSignalsStrings = new QString[MAX_SIGNALS_COUNT]();
    int signalsCounter = 0;

    if (data.isEmpty()) {
        return true;
    }

    QString currentSignal = "";
    QChar c;
    int commonCounter = 0;
    bool isDataWrong = false;
    bool isDataEnded = false;
    while (!isDataEnded && !isDataWrong) {
        currentSignal = "";
        int counter = 0;
        do {
            if (commonCounter == data.length()) {
                isDataEnded = true;
                break;
            }
            if (counter == MAX_SIGNAL_LENGTH + 1) {
                isDataWrong = true;
                break;
            }
            c = data[commonCounter++];
            if (c != ' ') {
                if (c != ',') {
                    currentSignal[counter] = c;
                }
                counter++;
            }
        } while (c != ',');
        if (commonCounter == data.length()) {
            isDataEnded = true;
        }

        if (currentSignal[0] != 'y' && currentSignal != "") {
            isDataWrong = true;
        }
        if (isDataEnded
                && c != ','
                && c != ' '
                && (counter == MAX_SIGNAL_LENGTH + 1
                    || c < '0'
                    || c > '9')) {
            isDataWrong = true;
        }
        for (int pos = 1; pos < counter - 1; pos++) {
            if (currentSignal[pos] < '0' || currentSignal[pos] > '9') {
                isDataWrong = true;
            }
        }
        if (currentSignal != "") {
            for (int i = 0; i < signalsCounter; i++) {
                if (currentSignal == controlSignalsStrings[i]) {
                    isDataWrong = true;
                    break;
                }
            }
        }

        if (isDataWrong == false && currentSignal != "") {
            controlSignalsStrings[signalsCounter++] = currentSignal;
        }
    }

    if (isDataWrong) {
        return false;
    } else {
        controlSignals = new int[MAX_SIGNALS_COUNT]();
        for (int i = 0; i < signalsCounter; i++) {
            controlSignalsStrings[i].remove(0,1);
            controlSignals[i] = controlSignalsStrings[i].toInt();
        }
        return true;
    }
}

void TTLineEdit::sortControlSignals() {
    QString controlSignalsString = this->text();
    if (controlSignalsString.isEmpty()) return;

    insertionSort(controlSignals, 0);

    controlSignalsString = "";
    int index = 0;
    while (controlSignals[index] != 0) {
        if ((controlSignals[index] >= MIN_SIGNAL_FLAG
                && controlSignals[index] <= MAX_SIGNAL_FLAG)
                || (controlSignals[index] >= MIN_SIGNAL_OPERATION
                    && controlSignals[index] <= MAX_SIGNAL_OPERATION)) {
            bool isNonexistentSignal = false;
            for (int i = 0; i < NONEXISTENT_SIGNALS_COUNT; i++)
                if (controlSignals[index] == NONEXISTENT_SIGNALS[i]) {
                    isNonexistentSignal = true;
                    break;
                }
            if (!isNonexistentSignal) {
                controlSignalsString += 'y';
                controlSignalsString += QString::number(controlSignals[index]);
                if (index <= MAX_SIGNALS_COUNT - 1
                        && controlSignals[index + 1] != 0) {
                    controlSignalsString += ',';
                }
            } else {
                if (controlSignals[index + 1] == 0
                        && !controlSignalsString.isEmpty()) {
                    controlSignalsString.remove(
                                controlSignalsString.length() - 1, 1
                                );
                }
            }
        } else {
            if (controlSignals[index + 1] == 0
                   && !controlSignalsString.isEmpty()) {
                controlSignalsString.remove(
                            controlSignalsString.length() - 1, 1
                            );
            }
        }
        index++;
    }
    this->setText(controlSignalsString);
}

void TTLineEdit::onStateEditingFinished() {
    int otherStateColumn = (column == INITIAL_STATE_COLUMN) ?
                            NEXT_STATE_COLUMN : INITIAL_STATE_COLUMN;
    TTLineEdit *otherState =
            (TTLineEdit*)mainWindow
            ->getTtField(row,otherStateColumn);
    TTLineEdit *transitionConditions =
            (TTLineEdit*)mainWindow
            ->getTtField(row, TRANSITION_CONDITIONS_COLUMN);
    TTLineEdit *controlSignals =
            (TTLineEdit*)mainWindow
            ->getTtField(row, CONTROL_SIGNALS_COLUMN);
    if (this->text().isEmpty()) {
        if ((!transitionConditions->text().isEmpty()
                || !controlSignals->text().isEmpty()
                || !otherState->text().isEmpty() )) {
            setIsCorrect(false);
        }
    } else {
        if (otherState->text().isEmpty()) {
            otherState->setIsCorrect(false);
        }
    }
}

void TTLineEdit::onStateChanged(const QString &data) {
    int otherStateColumn = (column == INITIAL_STATE_COLUMN) ?
                            NEXT_STATE_COLUMN : INITIAL_STATE_COLUMN;
    TTLineEdit *otherState =
            (TTLineEdit*)mainWindow
            ->getTtField(row,otherStateColumn);
    TTLineEdit *transitionConditions =
            (TTLineEdit*)mainWindow
            ->getTtField(row, TRANSITION_CONDITIONS_COLUMN);
    TTLineEdit *controlSignals =
            (TTLineEdit*)mainWindow
            ->getTtField(row, CONTROL_SIGNALS_COLUMN);
    if (!this->text().isEmpty()) {
        setIsCorrect(true);
    } else {
        if (transitionConditions->text().isEmpty()
                && controlSignals->text().isEmpty()) {
            otherState->setIsCorrect(true);
        }
    }
}

void TTLineEdit::onTransitionConditionsEditingFinished() {    
    if (!areTransitionConditionsCorrect()) {
        setIsCorrect(false);
    } else {
        sortTransitionConditions();
    }

    if (!this->text().isEmpty()) {
        TTLineEdit *initialState =
                (TTLineEdit*)mainWindow
                ->getTtField(row, INITIAL_STATE_COLUMN);
        TTLineEdit *nextState =
                (TTLineEdit*)mainWindow
                ->getTtField(row, NEXT_STATE_COLUMN);
        if (initialState->text().isEmpty()) {
            initialState->setIsCorrect(false);
        }
        if (nextState->text().isEmpty()) {
            nextState->setIsCorrect(false);
        }
    }
}

void TTLineEdit::onTransitionConditionsChanged(const QString &data) {
    if (areTransitionConditionsCorrect()) {
        setIsCorrect(true);
    }

    TTLineEdit *controlSignals =
            (TTLineEdit*)mainWindow
            ->getTtField(row, CONTROL_SIGNALS_COLUMN);
    if (this->text().isEmpty() && controlSignals->text().isEmpty()) {
        TTLineEdit *initialState =
                (TTLineEdit*)mainWindow
                ->getTtField(row, INITIAL_STATE_COLUMN);
        TTLineEdit *nextState =
                (TTLineEdit*)mainWindow
                ->getTtField(row, NEXT_STATE_COLUMN);
        if (nextState->text().isEmpty()) {
            initialState->setIsCorrect(true);
        }
        if (initialState->text().isEmpty()) {
            nextState->setIsCorrect(true);
        }
    }
}

void TTLineEdit::onControlSignalsEditingFinished() {
    if (!areControlSignalsCorrect()) {
        setIsCorrect(false);
    } else {
        sortControlSignals();
    }

    if (!this->text().isEmpty()) {
        TTLineEdit *initialState =
                (TTLineEdit*)mainWindow
                ->getTtField(row, INITIAL_STATE_COLUMN);
        TTLineEdit *nextState =
                (TTLineEdit*)mainWindow
                ->getTtField(row, NEXT_STATE_COLUMN);
        if (initialState->text().isEmpty()) {
            initialState->setIsCorrect(false);
        }
        if (nextState->text().isEmpty()) {
            nextState->setIsCorrect(false);
        }
    }
}

void TTLineEdit::onControlSignalsChanged(const QString &data) {
    if (areControlSignalsCorrect()) {
        setIsCorrect(true);
    }

    TTLineEdit *transitionConditions =
            (TTLineEdit*)mainWindow
            ->getTtField(row,TRANSITION_CONDITIONS_COLUMN);
    if (this->text().isEmpty() && transitionConditions->text().isEmpty()) {
        TTLineEdit *initialState =
                (TTLineEdit*)mainWindow
                ->getTtField(row, INITIAL_STATE_COLUMN);
        TTLineEdit *nextState =
                (TTLineEdit*)mainWindow
                ->getTtField(row, NEXT_STATE_COLUMN);
        initialState->setIsCorrect(true);
        nextState->setIsCorrect(true);
    }
}

void TTLineEdit::insertionSort(int *array,  int stopNumber) {
    for (int i = 1; array[i] != stopNumber; ++i) {
        int temp = array[i];
        int j;
        for (j = i; j > 0 && temp < array[j - 1]; --j) {
            array[j] = array[j - 1];
        }
        array[j] = temp;
    }
}

void TTLineEdit::insertionSort(int *array, QChar *signs, int stopNumber) {
    for (int i = 1; array[i] != stopNumber; i++) {
        int temp = array[i];
        QChar tempSign = signs[i];
        int j;
        for (j = i; j > 0 && temp < array[j - 1]; j--) {
            array[j] = array[j - 1];
            signs[j] = signs[j - 1];
        }
        array[j] = temp;
        signs[j] = tempSign;
    }
}

void TTLineEdit::deleteField() {
    this->destroy();
}

void TTLineEdit::setIsCorrect(bool isCorrect) {
    if (isCorrect) {
        if (!this->isCorrect) {
            this->setStyleSheet(TT_LINE_EDIT_STYLE);
            this->isCorrect = true;
            mainWindow->decrementNErrors();
        }
    } else {
        this->setStyleSheet(TT_LINE_EDIT_ERROR_STYLE);
        if (this->isCorrect) {
            this->isCorrect = false;
            mainWindow->incrementNErrors();
        }
    }
}

bool TTLineEdit::getIsCorrect() {
    return isCorrect;
}

int* TTLineEdit::getConditionsNumbers() {
    return conditionsNumbers;
}

bool* TTLineEdit::getConditionsSigns() {
    return conditionsSigns;
}

int* TTLineEdit::getControlSignals() {
    return controlSignals;
}
