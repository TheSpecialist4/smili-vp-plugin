#ifndef NODEBASE_H
#define NODEBASE_H

#include <QObject>
#include <QString>
#include <QList>

class NodeBase : public QObject
{
    Q_OBJECT

public:
    NodeBase(QString name);

    QString getName();

    void setName(QString name);

    QList<QString> getOutPlugs();

    QList<QString> getInPlugs();

protected:
    void addInPlug(QString inPlugName);
    void addOutPlug(QString outPlugName);

    void clearInPlugs();
    void clearOutPlugs();

private: //members
    QString name;

    QList<QString> inPlugs;
    QList<QString> outPlugs;
};

#endif // NODEBASE_H
