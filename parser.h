#ifndef PARSER_H
#define PARSER_H

#include <QList>
#include <QHash>
#include <QString>
#include <QQueue>

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
//    void parseStart(QList<NodeCtrl *> *queue, QList<NodeCtrl *> *processed);

//    void parsePython(QList<NodeCtrl*>* queue, QList<NodeCtrl*>* processed);

//    void parseValue(QList<NodeCtrl*>* queue, QList<NodeCtrl*>* processed);

    bool addNextNodeToQueue(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void parseNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void parseStartNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void parsePythonNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);
    void parsePythonPrint(NodeCtrl* node);

    void parseEndNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

private: //members

    ErrorChecker* error;

    QHash<zodiac::NodeHandle, NodeCtrl*> allNodesDict;

    QString* script;
};

#endif // PARSER_H
