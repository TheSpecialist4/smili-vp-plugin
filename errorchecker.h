#ifndef ERRORCHECKER_H
#define ERRORCHECKER_H

#include <QString>

class ErrorChecker
{
public:
    ErrorChecker();

    void isErrorPresent(bool error);

    QString getErrorMessage();

    void setErrorMessage(QString message);

    void appendToErrorMessage(QString message);

private:
    bool isError;
    QString* errorMessage;
};

#endif // ERRORCHECKER_H
