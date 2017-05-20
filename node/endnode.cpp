#include "endnode.h"

EndNode::EndNode(QString name, NodeType nodeType)
    : NodeBase(name, nodeType)
{
    this->clearOutPlugs();
}
