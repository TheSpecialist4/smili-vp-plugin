#include "startnode.h"

#include <QDebug>

StartNode::StartNode(QString name, NodeType nodeType)
    : NodeBase(name, nodeType)
{
    this->clearInPlugs();
}
