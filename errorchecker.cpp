#include "errorchecker.h"

ErrorChecker::ErrorChecker()
{
    errorMessage = new QString();
    isError = false;
}

void ErrorChecker::isErrorPresent(bool error) {
    this->isError = error;
}

QString ErrorChecker::getErrorMessage() {
    return *errorMessage;
}

void ErrorChecker::setErrorMessage(QString message) {
    this->errorMessage = new QString(message);
}

void ErrorChecker::appendToErrorMessage(QString message) {
    errorMessage->append(message);
}
