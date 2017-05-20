#include "parser.h"
#include "node/pythonnode.h"

#include <QDebug>

Parser::Parser(QHash<zodiac::NodeHandle, NodeCtrl *> allNodesDict) {
    this->allNodesDict = allNodesDict;
    error = new ErrorChecker();
    script = new QString();
}

QString Parser::getScript() {
    return *script;
}

ErrorChecker Parser::parseGraph(QList<NodeCtrl *> nodeCtrls) {
    QQueue<NodeCtrl*>* nodesQueue = new QQueue<NodeCtrl*>();
    for (NodeCtrl* node : nodeCtrls) {
        if (node->getName() == "START") {
            nodesQueue->enqueue(node);
            break;
        }
    }
    if (nodesQueue->isEmpty() || nodesQueue->size() > 1) {
        qDebug() << "error in finding START node. There can only be one START node\n";
        return *error;
    }
    while (!nodesQueue->isEmpty()) {
        parseNode(nodesQueue->dequeue(), nodesQueue);
    }
    return *error;
}

void Parser::parseNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    switch (node->getNodeModel()->getNodeType()) {
    case NodeType::START_NODE:
        parseStartNode(node, nodesQueue);
        break;
    case NodeType::END_NODE:
        parseEndNode(node, nodesQueue);
        break;
    case NodeType::FOR_NODE:
        //parse for
        break;
    case NodeType::IF_ELSE_NODE:
        //parse if else
        break;
    case NodeType::OP_NODE:
        //parse operator
        break;
    case NodeType::PYTHON_NODE:
        parsePythonNode(node, nodesQueue);
        break;
    case NodeType::VALUE_NODE:
        //parse value
        break;
    case NodeType::VAR_NODE:
        //parse variable
        break;
    default:
        break;
    }
}

bool Parser::addNextNodeToQueue(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "OUT PIPE") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "IN PIPE") {
                    //qDebug << "adding" << connectedPlug.getNode().getName() << "\n";
                    nodesQueue->enqueue(allNodesDict.find(connectedPlug.getNode()).value());
                    return true;
                }
            }
        }
    }
    return false;
}

void Parser::parseStartNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse START\n";
    if (!addNextNodeToQueue(node, nodesQueue)) {
        qDebug() << "pipeline disconnected here\n";
    }
    qDebug() << "end parse START\n";
}

void Parser::parseEndNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse END\n";
    qDebug() << "end parse END\n";
}

void Parser::parsePythonPrint(NodeCtrl* node) {
    script->append(QString("print("));
    NodeBase* printValueNode;
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "Print value") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "OUT PIPE") {
                    qDebug() << "out pipe can only be connected to in pipe";
                    return;
                }
                printValueNode = allNodesDict.find(connectedPlug.getNode()).value()->getNodeModel();
                break;
            }
            break;
        }
    }
    if (printValueNode->getNodeType() == NodeType::VALUE_NODE) {
        script->append(QString("\""));
        script->append(printValueNode->getName());
        script->append(QString("\""));
    }
}

void Parser::parsePythonNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse PYTHON\n";
    if (!addNextNodeToQueue(node, nodesQueue)) {
        qDebug() << "pipeline disconnected here\n";
    }
    QString funcName = ((PythonNode*)node->getNodeModel())->getFuncName();
    if (funcName == "print") {
        parsePythonPrint(node);
    }
    qDebug() << "end parse PYTHON\n";
}

/*ErrorChecker Parser::parseGraph(QList<NodeCtrl *> nodeCtrls) {
    QList<NodeCtrl*>* queue = new QList<NodeCtrl*>();
    QList<NodeCtrl*>* processed = new QList<NodeCtrl*>();

    for (NodeCtrl* node : nodeCtrls) {
        qDebug() << "Checking " << node->getName();
        if (node->getName() == "START") {
            qDebug() << "Start added\n";
            queue->append(node);
            break;
        }
    }

    if (queue->isEmpty()) {
        error->setErrorMessage("Error: No START NODE found");
        return *error;
    }

    while (!queue->isEmpty()) {
        switch (queue->at(0)->getNodeModel()->getNodeType()) {
//        case NodeType::FOR_NODE:
//            //parse for
//            break;
//        case NodeType::IF_ELSE_NODE:
//            //parse if else
//            break;
//        case NodeType::OP_NODE:
//            //parse op node
//            break;
        case NodeType::PYTHON_NODE:
            parsePython(queue, processed);
            break;
        case NodeType::START_NODE:
            parseStart(queue, processed);
            break;
//        case NodeType::VALUE_NODE:
//            //parse value
//            break;
//        case NodeType::VAR_NODE:
//            //parse variable
//            break;
        default:
            error->appendToErrorMessage(QString("Parsing error: Undefined node"));
            //undefined
            break;
        }
    }
    error->appendToErrorMessage(QString("\nParsing complete"));
    return *error;
}

void Parser::parseStart(QList<NodeCtrl *>* queue, QList<NodeCtrl *>* processed) {
    qDebug() << "Parsing StartStop length: " << queue->length();
    auto plugs = queue->at(0)->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "OUT PIPE") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "IN PIPE") {
                    queue->append(allNodesDict.find(connectedPlug.getNode()).value());
                }
            }
        } else if (plug.getName() == "IN PIPE") {
            script->append(QString("\n"));
        }
    }
    processed->append(queue->at(0));
    queue->removeFirst();
    qDebug() << "End Parse StartStop length: " << queue->length();
}

void Parser::parsePython(QList<NodeCtrl *> *queue, QList<NodeCtrl *> *processed) {
    qDebug() << "Parsing Python length: " << queue->length();
    auto plugs = queue->at(0)->getPlugHandles();
    bool isError = true;
    for (auto plug : plugs) {
        if (plug.getName() == "OUT PIPE") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "IN PIPE") {
                    queue->append(allNodesDict.find(connectedPlug.getNode()).value());
                    qDebug() << "In pipe from python added";
                    isError = false;
                    break;
                } else {
                    isError = true;
                }
            }
        }
        if (plug.getName() == "Print value") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "My Value") {
                    processed->append(allNodesDict.find(connectedPlug.getNode()).value());
                    script->append(QString(
                                       "print(" +
                                       allNodesDict.find(connectedPlug.getNode()).value()->getName()) +
                                        ")");
                    isError = false;
                    break;
                } else {
                    isError = true;
                }
            }
        }
    }
    if (isError) {
        error->appendToErrorMessage(QString("Error in parsing Python Print node \n"));
    }
    processed->append(queue->at(0));
    queue->removeFirst();
    qDebug() << "End Parse Python Print length: " << queue->length();
}

void Parser::parseValue(QList<NodeCtrl *> *queue, QList<NodeCtrl *> *processed) {

}
*/
