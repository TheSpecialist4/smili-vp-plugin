#ifndef OPERATIONNODE_H
#define OPERATIONNODE_H

class QString;

#include "node/nodebase.h"

class ImageOperationNode : public NodeBase
{
public:
    ImageOperationNode(QString name, NodeType nodeType, QString funcName);

    QString getFuncName();

private: //functions
    void createVectorFieldPlugs();

    void createMedianPlugs();

    void createStreamLinesPlugs();

    void createPseudoImagePlugs();

    void createScalePlugs();

private: //members
    QString funcName;
};

#endif // OPERATIONNODE_H
