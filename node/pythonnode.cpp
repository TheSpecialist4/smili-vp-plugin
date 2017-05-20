#include "pythonnode.h"

PythonNode::PythonNode(QString name, NodeType type, QString funcName)
  : NodeBase(name, type)
{
    this->funcName = funcName;
    this->addInPlug(QString("Print value"));
}
