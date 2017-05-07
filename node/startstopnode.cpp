#include "startstopnode.h"

#include <QDebug>

StartStopNode::StartStopNode(QString name, bool isStart, NodeType nodeType)
    : NodeBase(name, nodeType)
{
    this->isStart = isStart;

    if (isStart) {
        this->clearInPlugs();
    } else {
        this->clearOutPlugs();
    }
}
