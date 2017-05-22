#include "node/variablenode.h"

VariableNode::VariableNode(QString name, NodeType nodeType)
 : NodeBase(name, nodeType)
{
    this->addInPlug(QString("SET"));
    this->addOutPlug(QString("GET"));
}

void VariableNode::setValue(QString value) {
    this->value = value;
}

QString VariableNode::getValue() {
    return value;
}

