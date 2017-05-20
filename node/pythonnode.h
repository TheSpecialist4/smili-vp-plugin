#ifndef PYTHONPRINT_H
#define PYTHONPRINT_H

#include "nodebase.h"
#include <QString>
#include "nodetype.h"

class PythonNode : public NodeBase
{
public:
    PythonNode(QString name, NodeType type, QString funcName);

    QString getFuncName();

private: //functions
    void createPrintPlugs();

    void createExecPlugs();

private: //members
    QString funcName;
};

#endif // PYTHONPRINT_H
