#include "fornode.h"

ForNode::ForNode(QString name, NodeType nodeType)
    : NodeBase(name, nodeType)
{
    this->addInPlug(QString("Variable"));
    this->addInPlug(QString("Collection"));
    this->addInPlug(QString("EndFor"));
    this->addOutPlug(QString("StartFor"));
}
