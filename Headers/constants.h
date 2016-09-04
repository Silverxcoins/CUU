#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "mainwindow.h"
#include <QString>

#define MIN_STATE 0
#define MAX_STATE 999

#define INITIAL_STATE_COLUMN 2
#define NEXT_STATE_COLUMN 4
#define TRANSITION_CONDITIONS_COLUMN 6
#define CONTROL_SIGNALS_COLUMN 8

#define MAX_ROWS_COUNT 499
#define MAX_ROWS_ON_SCREEN 17

#define POSSIBLE_CONDITIONS_COUNT 8
#define MAX_CONDITIONS_COUNT 8

#define NONEXISTENT_SIGNALS_COUNT 12
#define MAX_SIGNAL_LENGTH 3
#define MAX_SIGNALS_COUNT 11
#define MIN_SIGNAL_FLAG 6
#define MAX_SIGNAL_FLAG 15
#define MIN_SIGNAL_OPERATION 20
#define MAX_SIGNAL_OPERATION 70

#define RAM_ROWS_COUNT 16
#define RAM_COLUMNS_COUNT 16

#define HEX 16
#define BIN 2
#define BIN_LENGTH 8

#define MAX_UNSIGNED_HEX_VALUE 255
#define MIN_SIGNED_HEX_VALUE -128
#define MAX_SIGNED_HEX_VALUE 127

#define IMPOSSIBLE_VALUE -1000

#define RK_SIZE 4
#define RON_NUMBER 16

#define MAX_REWIND_STEP 1000

static const QString CONTROL_SEQUENCE = "SAA";
static const QChar CONTROL_SYMBOL = '\n';

static const QString POSSIBLE_CONDITIONS[] = {"x4", "x3", "x2", "x1",
                                              "z", "n", "c", "p"};

static const int NONEXISTENT_SIGNALS[] = {32, 33, 34, 39, 40, 44,
                                          46, 51, 54, 59, 61, 65};

#endif // CONSTANTS_H
