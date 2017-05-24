#include "mainwindownode.h"

MainWindowNode::MainWindowNode(QString name, NodeType nodeType, QString funcName)
    : NodeBase(name, nodeType)
{
    this->funcName = funcName;

    if (funcName == "loadFile") {
        this->addInPlug(QString("fileName"));
        this->addOutPlug(QString("Result"));
    } else if (funcName == "saveScreen") {
        this->addInPlug(QString("fileName"));
    } else if (funcName == "activeModel" || funcName == "activeImage") {
        this->addOutPlug(QString("Result"));
    }
}

QString MainWindowNode::getFuncName() {
    return this->funcName;
}
