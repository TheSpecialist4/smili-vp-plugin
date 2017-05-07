#include "fornode.h"

ForNode::ForNode(QString name, NodeType nodeType)
    : NodeBase(name, nodeType)
{
    this->addInPlug(QString("Statement"));
    this->addInPlug(QString("Condition"));
    this->addInPlug(QString("Update Expression"));
}
