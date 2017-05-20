#include "pythonnode.h"

PythonNode::PythonNode(QString name, NodeType type, QString funcName)
  : NodeBase(name, type)
{
    this->funcName = funcName;
    if (funcName == "print") {
        createPrintPlugs();
    }
}

QString PythonNode::getFuncName() {
    return this->funcName;
}

void PythonNode::createPrintPlugs() {
    this->addInPlug(QString("Print value"));
}
