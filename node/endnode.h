#ifndef STOPNODE_H
#define STOPNODE_H

class QString;

#include "node/nodebase.h"

class EndNode : public NodeBase
{
public:
    EndNode(QString name, NodeType nodeType);
};

#endif // STOPNODE_H
