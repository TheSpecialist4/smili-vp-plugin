#ifndef STARTSTOPNODE_H
#define STARTSTOPNODE_H

class QString;

#include "node/nodebase.h"

class StartNode : public NodeBase
{
public:
    StartNode(QString name, NodeType nodeType);
};

#endif // STARTSTOPNODE_H
