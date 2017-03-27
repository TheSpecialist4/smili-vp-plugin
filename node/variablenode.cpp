#include "node/variablenode.h"

VariableNode::VariableNode(QString name)
 : NodeBase(name)
{
    this->addInPlug(QString("InPlug 1"));
    this->addOutPlug(QString("OutPlug 1"));

    isImage = true;
}

void VariableNode::isImageNode(bool isImage) {
    this->isImage = isImage;
}
