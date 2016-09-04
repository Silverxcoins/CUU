#include "mainwindow.h"
#include "ramlineedit.h"
#include "styles.h"
#include "constants.h"

RamLineEdit::RamLineEdit(QTableWidget *table, int row, int column) :
        QLineEdit() {
    this->table = table;
    this->row = row;
    this->column = column;
    this->setText("00");
    this->setAlignment(Qt::AlignHCenter);
    this->setInputMask(">HH");
    if (row % 2 == 0) {
        this->setStyleSheet(RAM_EVEN_STYLE);
    } else {
        this->setStyleSheet(RAM_ODD_STYLE);
    }
    MainWindow::connect(this, SIGNAL(textChanged(const QString &)),
                        this, SLOT(onTextChanged(const QString &)));
    table->setCellWidget(row, column, this);
}

RamLineEdit::~RamLineEdit() {
}

void RamLineEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up) {
        if (row > 0) {
            ((RamLineEdit*)table->cellWidget(row - 1, column))
                               ->setCursorPosition(this->cursorPosition());
            ((RamLineEdit*)table->cellWidget(row - 1, column))->setFocus();
        }
    }
    else if (event->key() == Qt::Key_Down) {
        if (row < RAM_ROWS_COUNT - 1) {
            ((RamLineEdit*)table->cellWidget(row + 1, column))
                               ->setCursorPosition(this->cursorPosition());
            ((RamLineEdit*)table->cellWidget(row + 1, column))->setFocus();
        }
    }
    else if (event->key() == Qt::Key_Left) {
        if (column > 0 && this->cursorPosition() == 0) {
            ((RamLineEdit*)table->cellWidget(row, column - 1))
                               ->setCursorPosition(1);
            ((RamLineEdit*)table->cellWidget(row, column - 1))->setFocus();
        } else {
            QLineEdit::keyPressEvent(event);
        }
    }
    else if (event->key() == Qt::Key_Right) {
        if (column < RAM_COLUMNS_COUNT - 1 && this->cursorPosition() > 0) {
            ((RamLineEdit*)table->cellWidget(row, column + 1))
                               ->setCursorPosition(0);
            ((RamLineEdit*)table->cellWidget(row, column + 1))->setFocus();
        } else {
            QLineEdit::keyPressEvent(event);
        }
    }
    else if (event->key() == Qt::Key_Backspace) {
        int cursorPosition = this->cursorPosition();
        if (cursorPosition == 2) {
            cursorPosition--;
        }
        QString text = this->text();
        text[cursorPosition] = '0';
        this->setText(text);
        if (cursorPosition > 0) {
            this->setCursorPosition(0);
            this->setFocus();
        } else if (column > 0) {
            ((RamLineEdit*)table->cellWidget(row, column - 1))
                               ->setCursorPosition(1);
            ((RamLineEdit*)table->cellWidget(row, column - 1))->setFocus();
        } else if (row > 0) {
            ((RamLineEdit*)table->cellWidget(row - 1, RAM_COLUMNS_COUNT - 1))
                               ->setCursorPosition(1);
            ((RamLineEdit*)table->cellWidget(row - 1, RAM_COLUMNS_COUNT - 1))->setFocus();
        } else {
            this->setCursorPosition(cursorPosition);
            this->setFocus();
        }
    } else if (event->key() == Qt::Key_Delete) {
    } else {
        QLineEdit::keyPressEvent(event);
    }
}

void RamLineEdit::onTextChanged(const QString &data) {
    if (this->cursorPosition() == 2) {
        if (column < RAM_COLUMNS_COUNT - 1) {
            ((RamLineEdit*)table->cellWidget(row, column + 1))
                               ->setCursorPosition(0);
            ((RamLineEdit*)table->cellWidget(row, column + 1))->setFocus();
        } else if (row < RAM_ROWS_COUNT - 1) {
            ((RamLineEdit*)table->cellWidget(row + 1, 0))
                               ->setCursorPosition(0);
            ((RamLineEdit*)table->cellWidget(row + 1, 0))->setFocus();
        }
    }
}
