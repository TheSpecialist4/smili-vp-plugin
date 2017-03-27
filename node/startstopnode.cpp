#include "startstopnode.h"

#include <QDebug>

StartStopNode::StartStopNode(QString name, bool isStart)
    : NodeBase(name)
{
    this->isStart = isStart;

    if (isStart) {
        this->clearInPlugs();
    } else {
        this->clearOutPlugs();
    }
}
