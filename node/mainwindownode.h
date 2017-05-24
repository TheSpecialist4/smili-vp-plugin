#ifndef MAINWINDOWNODE_H
#define MAINWINDOWNODE_H

#include <QString>

#include "nodebase.h"

class MainWindowNode : public NodeBase
{
public:
    MainWindowNode(QString name, NodeType nodeType, QString funcName);

    QString getFuncName();

private: //members
    QString funcName;
};

#endif // MAINWINDOWNODE_H
