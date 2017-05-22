#ifndef VARIABLENODE_H
#define VARIABLENODE_H

#include "node/nodebase.h"

#include <QString>

class VariableNode : public NodeBase
{
public:
    VariableNode(QString name, NodeType nodeType);

    QString getValue();

    void setValue(QString);

private: //private members
    QString value;
};

#endif // VARIABLENODE_H
