#ifndef PARSER_H
#define PARSER_H

#include <QList>
#include <QHash>
#include <QString>

#include "errorchecker.h"
#include "nodectrl.h"
#include "zodiacgraph/nodehandle.h"

class Parser : QObject
{
public:
    ErrorChecker parseGraph(QList<NodeCtrl*> nodeCtrls);

    Parser(QHash<zodiac::NodeHandle, NodeCtrl*>);

    QString getScript();

private:
    void parseStartStop(QList<NodeCtrl *> *queue, QList<NodeCtrl *> *processed);

    void parsePythonPrint(QList<NodeCtrl*>* queue, QList<NodeCtrl*>* processed);

    //void parseValue(QList<NodeCtrl*>* queue, QList<NodeCtrl*>* processed);

    ErrorChecker* error;

    QHash<zodiac::NodeHandle, NodeCtrl*> allNodesDict;

    QString* script;
};

#endif // PARSER_H
