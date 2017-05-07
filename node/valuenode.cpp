#include "valuenode.h"

ValueNode::ValueNode(QString name, NodeType nodeType)
    : NodeBase(name, nodeType)
{
    /* this node can't have any pipeline plugs */
    this->clearInPlugs();
    this->clearOutPlugs();

    this->addOutPlug(QString("My Value"));
}
