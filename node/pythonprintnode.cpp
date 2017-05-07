#include "pythonprintnode.h"

PythonPrintNode::PythonPrintNode(QString name, NodeType type)
  : NodeBase(name, type)
{
    this->addInPlug(QString("Print value"));
}
