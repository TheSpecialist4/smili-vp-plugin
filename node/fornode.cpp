#include "fornode.h"

ForNode::ForNode(QString name)
    : NodeBase(name)
{
    this->addInPlug(QString("Statement"));
    this->addInPlug(QString("Condition"));
    this->addInPlug(QString("Update Expression"));
}
