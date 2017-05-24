#ifndef PARSER_H
#define PARSER_H
#include <QDataStream>
#include <QList>
#include <QHash>
#include <QString>
#include <QQueue>
#include <QFile>
#include <QMessageBox>

#include "errorchecker.h"
#include "nodectrl.h"
#include "zodiacgraph/nodehandle.h"

#include "node/pythonnode.h"
#include "node/imageoperationnode.h"
#include "node/variablenode.h"
#include "node/mainwindownode.h"

class Parser : QObject
{
public:
    ErrorChecker parseGraph(QList<NodeCtrl*> nodeCtrls);

    Parser(QHash<zodiac::NodeHandle, NodeCtrl*>);

    QString getScript();

    ErrorChecker* getErrorHandler();

private:
//    void parseStart(QList<NodeCtrl *> *queue, QList<NodeCtrl *> *processed);

//    void parsePython(QList<NodeCtrl*>* queue, QList<NodeCtrl*>* processed);

//    void parseValue(QList<NodeCtrl*>* queue, QList<NodeCtrl*>* processed);

    bool addNextNodeToQueue(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void parseNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void parseStartNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void parsePythonNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);
    void parsePythonPrint(NodeCtrl* node);

    void parseVariableNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void parseImageOperationNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);
    void parseImageOpScale(NodeCtrl* node);
    void parseImageOpVectorField(NodeCtrl* node);
    void parseImageOpMedian();
    void parseImageOpStreamLines();
    void parseImageOpPseudoImage();

    void parseMainWindowNode(NodeCtrl* node,QQueue<NodeCtrl*>* nodesQueue);
    void parseMWLoadFile(NodeCtrl* node);
    void parseMWSaveScreen(NodeCtrl* node);

    void parseEndNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void parseForNode(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);
    bool addForStartNodeToQueue(NodeCtrl* node, QQueue<NodeCtrl*>* nodesQueue);

    void addIndentationToLine();

private: //members

    ErrorChecker* error;

    QHash<zodiac::NodeHandle, NodeCtrl*> allNodesDict;

    QString* script;

    QDataStream out;

    QSet<NodeCtrl*>* indentationLevel;

    QString fourSpaces;
};

#endif // PARSER_H
