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

ErrorChecker *Parser::getErrorHandler() {
    return this->error;
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
        error->appendToErrorMessage(QString("error in finding START node. There can only be one START node\n"));
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
    case NodeType::IMAGE_OP_NODE:
        parseImageOperationNode(node, nodesQueue);
        break;
    case NodeType::PYTHON_NODE:
        parsePythonNode(node, nodesQueue);
        break;
    case NodeType::VALUE_NODE:
        //parse value
        break;
    case NodeType::VAR_NODE:
        parseVariableNode(node, nodesQueue);
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
                } else {
                    error->appendToErrorMessage(QString("error at node "));
                    error->appendToErrorMessage(node->getName());
                    error->appendToErrorMessage(QString(". OUT PIPE can only be connected to IN PIPE\n"));
                }
            }
        }
    }
    return false;
}

void Parser::parseStartNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse START\n";
    if (!addNextNodeToQueue(node, nodesQueue)) {
        error->appendToErrorMessage(QString("pipeline disconnected at START node\n"));
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
                    error->appendToErrorMessage(QString("Error connecting the pipeline in PYTHON.PRINT node"
                                 "OUT PIPE can only be connected to IN PIPE\n"));
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
        script->append(QString("\")"));
    } else if (printValueNode->getNodeType() == NodeType::VAR_NODE) {
        script->append(printValueNode->getName());
        script->append(QString(")"));
    }
    script->append("\n");
}

void Parser::parsePythonNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse PYTHON\n";
    if (!addNextNodeToQueue(node, nodesQueue)) {
        error->appendToErrorMessage(QString("pipeline disconnected at PYTHON node\n"));
    }
    QString funcName = ((PythonNode*)node->getNodeModel())->getFuncName();
    if (funcName == "print") {
        parsePythonPrint(node);
    }
    qDebug() << "end parse PYTHON\n";
}

void Parser::parseVariableNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse VARIABLE";
    if (!addNextNodeToQueue(node, nodesQueue)) {
        error->appendToErrorMessage(QString("pipeline disconnected at VARIABLE node\n"));
    }
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "SET") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "My Value") {
                    qDebug() << "set value to variable";
                    ((VariableNode*)node->getNodeModel())->setValue(
                                allNodesDict.find(connectedPlug.getNode()).value()->getName());
                    script->append(node->getName());
                    script->append(QString(" = \""));
                    script->append(((VariableNode*)node->getNodeModel())->getValue());
                    script->append(QString("\"\n"));
                    break;
                } else if (connectedPlug.getName() == "GET") {
                    script->append(node->getName());
                    script->append(QString(" = "));
                    script->append(allNodesDict.find(connectedPlug.getNode()).value()->getName());
                    script->append(QString("\n"));
                    break;
                }
                else {
                    ((VariableNode*)node->getNodeModel())->setValue("");
                }
            }
            break;
        }
    }
    qDebug() << "end parse VARIABLE";
}

void Parser::parseImageOperationNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse IMAGE OPERATION";
    bool isVariableNodeConnected = false;
    addNextNodeToQueue(node, nodesQueue);
    QString funcName = ((ImageOperationNode*)node->getNodeModel())->getFuncName();

    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "Image Variable") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "Get") {
                    isVariableNodeConnected = true;
                    script->append(connectedPlug.getNode().getName());
                    script->append(QString("."));
                    break;
                }
            }
            if (!isVariableNodeConnected) {
                error->appendToErrorMessage(QString("variable node needs to be connected to an IMAGE OPERATION"));
            }
            break;
        }
    }
    if (funcName == "scale") {
        parseImageOpScale(node);
    } else if (funcName == "vectorField") {
        parseImageOpVectorField(node);
    } else if (funcName == "median") {
        parseImageOpMedian();
    } else if (funcName == "pseudoImage") {
        parseImageOpPseudoImage();
    } else if (funcName == "streamLines") {
        parseImageOpStreamLines();
    }
    qDebug() << "end parse IMAGE OPERATION";
}

void Parser::parseImageOpScale(NodeCtrl *node) {
    script->append(QString("scale("));
    bool isCorrectNodeConnected = false;
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "Scaling Value") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "Get" || connectedPlug.getName() == "My Value") {
                    script->append(connectedPlug.getNode().getName());
                    script->append(QString(")\n"));
                    isCorrectNodeConnected = true;
                    break;
                }
            }
            if (!isCorrectNodeConnected) {
                error->appendToErrorMessage(QString("wrong type of plug connected to scaling"
                                                    "value in SCALE IMAGE OPERATION"));
            }
            break;
        }
    }
}

void Parser::parseImageOpMedian() {
    script->append(QString("median()\n"));
}

void Parser::parseImageOpPseudoImage() {
    script->append(QString("pseudoImage()\n"));
}

void Parser::parseImageOpStreamLines() {
    script->append(QString("streamLines()\n"));
}

void Parser::parseImageOpVectorField(NodeCtrl *node) {
    script->append(QString("vectorField("));

    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "subSampleFactor") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "Get" || connectedPlug.getName() == "My Value") {
                    script->append(connectedPlug.getNode().getName());
                    break;
                }
            }
        } else if (plug.getName() == "scaling") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "Get" || connectedPlug.getName() == "My Value") {
                    script->append(QString(", "));
                    script->append(connectedPlug.getNode().getName());
                    break;
                }
            }
        }
    }
    script->append(QString(")\n"));
}
