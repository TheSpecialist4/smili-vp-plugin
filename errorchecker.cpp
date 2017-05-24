#include "errorchecker.h"

ErrorChecker::ErrorChecker()
{
    errorMessage = new QString("Errors:\n");
    isError = false;
    errorCounter = 0;
}

bool ErrorChecker::isErrorPresent() {
    return this->isError;
}

void ErrorChecker::setError(bool error) {
    this->isError = error;
}

QString ErrorChecker::getErrorMessage() {
    return *errorMessage;
}

void ErrorChecker::setErrorMessage(QString message) {
    this->errorMessage = new QString(message);
}

void ErrorChecker::appendToErrorMessage(QString message) {
    isError = true;
    *errorMessage += QString::number(++errorCounter);
    errorMessage->append(": ");
    errorMessage->append(message);
}
