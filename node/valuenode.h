#ifndef VALUENODE_H
#define VALUENODE_H

#include "nodebase.h"

class ValueNode : public NodeBase
{
public:
    ValueNode(QString name, NodeType nodeType);
};

#endif // VALUENODE_H
