#include <statevalidator.h>
#include "constants.h"

StateValidator::StateValidator() : QValidator() {}

StateValidator::~StateValidator() {}

QValidator::State StateValidator::validate(QString &input, int &pos) const {
    if (input.isEmpty()) return Acceptable;

    bool isNumber;
    int value = input.toInt(&isNumber);
    if ((isNumber) && (value >= MIN_STATE) && (value <= MAX_STATE)) {
        return Acceptable;
    }

    return Invalid;
}
