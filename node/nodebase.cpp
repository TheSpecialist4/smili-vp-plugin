#include "node/nodebase.h"

NodeBase::NodeBase(QString name, NodeType nodeType)
    : name(name),
      inPlugs(QList<QString>()),
      outPlugs(QList<QString>()),
      nodeType(nodeType)
{
    inPlugs.append(QString("IN PIPE"));
    outPlugs.append(QString("OUT PIPE"));
}

QString NodeBase::getName() {
    return name;
}

void NodeBase::setName(QString name) {
    this->name = name;
}

void NodeBase::addInPlug(QString inPlugName) {
    inPlugs.append(inPlugName);
}

void NodeBase::addOutPlug(QString outPlugName) {
    outPlugs.append(outPlugName);
}

void NodeBase::clearInPlugs() {
    this->inPlugs.clear();
}

void NodeBase::clearOutPlugs() {
    this->outPlugs.clear();
}

QList<QString> NodeBase::getInPlugs() {
    return inPlugs;
}

QList<QString> NodeBase::getOutPlugs() {
    return outPlugs;
}

NodeType NodeBase::getNodeType() {
    return nodeType;
}
