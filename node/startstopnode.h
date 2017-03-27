#ifndef STARTSTOPNODE_H
#define STARTSTOPNODE_H

class QString;

#include "node/nodebase.h"

class StartStopNode : public NodeBase
{
public:
    StartStopNode(QString name, bool isStart);

private:
    bool isStart;
};

#endif // STARTSTOPNODE_H
