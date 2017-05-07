#ifndef NODEBASE_H
#define NODEBASE_H

#include <QObject>
#include <QString>
#include <QList>

#include "nodetype.h"

class NodeBase : public QObject
{
    Q_OBJECT

public:
    NodeBase(QString name, NodeType nodeType);

    QString getName();

    void setName(QString name);

    QList<QString> getOutPlugs();

    QList<QString> getInPlugs();

    NodeType getNodeType();

protected:
    void addInPlug(QString inPlugName);
    void addOutPlug(QString outPlugName);

    void clearInPlugs();
    void clearOutPlugs();

private: //members
    QString name;

    QList<QString> inPlugs;
    QList<QString> outPlugs;

    NodeType nodeType;
};

#endif // NODEBASE_H
