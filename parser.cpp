#include "parser.h"

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
        case NodeType::PYTHON_PRINT_NODE:
            parsePythonPrint(queue, processed);
            break;
        case NodeType::START_STOP_NODE:
            parseStartStop(queue, processed);
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

void Parser::parseStartStop(QList<NodeCtrl *>* queue, QList<NodeCtrl *>* processed) {
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

void Parser::parsePythonPrint(QList<NodeCtrl *> *queue, QList<NodeCtrl *> *processed) {
    qDebug() << "Parsing Python print length: " << queue->length();
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

//void Parser::parseValue(QList<NodeCtrl *> *queue, QList<NodeCtrl *> *processed) {

//}
