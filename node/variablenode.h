#ifndef VARIABLENODE_H
#define VARIABLENODE_H

#include "node/nodebase.h"

#include <QString>

class VariableNode : public NodeBase
{
public:
    VariableNode(QString name, NodeType nodeType);

    void isImageNode(bool isImage);

private: //private members
    bool isImage;
};

#endif // VARIABLENODE_H
