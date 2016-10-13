#include "nodemetadata.h"

NodeMetadata::NodeMetadata()
{

}

void NodeMetadata::setName(QString name)
{
    this->name = name;
}

void NodeMetadata::addInPlug(QString inPlugName)
{
    this->inPlugs.append(inPlugName);
}

void NodeMetadata::addOutPlug(QString outPlugName)
{
    this->outPlugs.append(outPlugName);
}

QList<QString> NodeMetadata::getInPlugs()
{
    return this->inPlugs;
}

QList<QString> NodeMetadata::getOutPlugs()
{
    return this->outPlugs;
}
