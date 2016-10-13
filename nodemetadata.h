#ifndef NODEMETADATA_H
#define NODEMETADATA_H

#include <QString>
#include <QList>

class NodeMetadata
{
public:
    NodeMetadata();

    QList<QString> getInPlugs();

    QList<QString> getOutPlugs();

    QString getName();

    void addInPlug(QString inPlugName);

    void addOutPlug(QString outPlugName);

    void setName(QString name);

private:

    QList<QString> inPlugs;

    QList<QString> outPlugs;

    QString name;
};

#endif // NODEMETADATA_H
