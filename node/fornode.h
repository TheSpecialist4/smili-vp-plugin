#ifndef FORNODE_H
#define FORNODE_H

class QString;

#include "node/nodebase.h"

class ForNode : public NodeBase
{
public:
    ForNode(QString name);
};

#endif // FORNODE_H
