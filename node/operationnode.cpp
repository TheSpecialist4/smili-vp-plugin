#include "operationnode.h"

OperationNode::OperationNode(QString name)
    : NodeBase(name)
{
    isImage = true;

    this->addInPlug(QString("InPlug 1"));
    this->addOutPlug(QString("OutPlug 1"));
}

void OperationNode::isImageNode(bool isImage) {
    this->isImage = isImage;
}
