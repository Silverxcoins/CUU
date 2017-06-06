#ifndef STATEVALIDATOR_H
#define STATEVALIDATOR_H
#include <QValidator>
#include <QObject>

class StateValidator : public QValidator {
public:
    StateValidator();

    ~StateValidator();

    virtual State validate( QString & input, int & pos ) const;

};

#endif // STATEVALIDATOR_H
