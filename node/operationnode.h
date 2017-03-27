#ifndef OPERATIONNODE_H
#define OPERATIONNODE_H

class QString;

#include "node/nodebase.h"

class OperationNode : public NodeBase
{
public:
    OperationNode(QString name);

    void isImageNode(bool isImage);

private:
    bool isImage;
};

#endif // OPERATIONNODE_H
