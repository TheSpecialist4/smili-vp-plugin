#ifndef PYTHONPRINT_H
#define PYTHONPRINT_H

#include "nodebase.h"
#include <QString>
#include "nodetype.h"

class PythonPrintNode : public NodeBase
{
public:
    PythonPrintNode(QString name, NodeType type);
};

#endif // PYTHONPRINT_H
