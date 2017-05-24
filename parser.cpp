#include "parser.h"

#include <QDebug>

Parser::Parser(QHash<zodiac::NodeHandle, NodeCtrl *> allNodesDict) {
    this->allNodesDict = allNodesDict;
    error = new ErrorChecker();
    script = new QString();
    fourSpaces = QString("    ");
    indentationLevel = new QSet<NodeCtrl*>();
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
        nodesQueue->clear();
        break;
    case NodeType::FOR_NODE:
        parseForNode(node, nodesQueue);
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
    case NodeType::MAIN_WINDOW_NODE:
        parseMainWindowNode(node, nodesQueue);
    default:
        break;
    }
}

bool Parser::addNextNodeToQueue(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "OUT PIPE") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "IN PIPE" || connectedPlug.getName() == "EndFor"
                        || connectedPlug.getName() == "EndIf") {
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
    addIndentationToLine();
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
    addIndentationToLine();
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "SET") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "My Value") { // connected to Constant (ValueNode)
                    qDebug() << "set value to variable";
                    ((VariableNode*)node->getNodeModel())->setValue(
                                allNodesDict.find(connectedPlug.getNode()).value()->getName());
                    script->append(node->getName());
                    script->append(QString(" = \""));
                    script->append(((VariableNode*)node->getNodeModel())->getValue());
                    script->append(QString("\"\n"));
                    break;
                } else if (connectedPlug.getName() == "GET") { // connected to a variable
                    script->append(node->getName());
                    script->append(QString(" = "));
                    script->append(allNodesDict.find(connectedPlug.getNode()).value()->getName());
                    script->append(QString("\n"));
                    break;
                } else if (connectedPlug.getName() == "Result") { //connected to Function with return value
                    script->append(node->getName());
                    script->append(" = ");
                    parseMainWindowNode(allNodesDict.find(connectedPlug.getNode()).value(), nodesQueue);
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

    addIndentationToLine();

    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "Image Variable") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "GET") {
                    isVariableNodeConnected = true;
                    script->append(connectedPlug.getNode().getName());
                    script->append(QString("."));
                    break;
                }
            }
            if (!isVariableNodeConnected) {
                error->appendToErrorMessage(QString("variable node needs to be connected to an IMAGE OPERATION\n"));
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
                if (connectedPlug.getName() == "GET" || connectedPlug.getName() == "My Value") {
                    script->append(connectedPlug.getNode().getName());
                    script->append(QString(")\n"));
                    isCorrectNodeConnected = true;
                    break;
                }
            }
            if (!isCorrectNodeConnected) {
                error->appendToErrorMessage(QString("wrong type of plug connected to scaling "
                                                    "value in SCALE IMAGE OPERATION\n"));
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
                if (connectedPlug.getName() == "GET" || connectedPlug.getName() == "My Value") {
                    qDebug() << "adding subSampleFactor";
                    script->append(connectedPlug.getNode().getName());
                    break;
                }
            }
        }
    }
    // done separately to ensure scaling is parsed after subSampleFactor
    for (auto plug : plugs) {
        if (plug.getName() == "scaling") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "GET" || connectedPlug.getName() == "My Value") {
                    qDebug() << "adding scaling";
                    script->append(", ");
                    script->append(connectedPlug.getNode().getName());
                    break;
                }
            }
        }
    }
    script->append(QString(")\n"));
}

void Parser::addIndentationToLine() {
    qDebug() << indentationLevel->size();
    for (int i = 0; i < indentationLevel->size(); i++) {
        script->append(fourSpaces);
    }
}

bool Parser::addForStartNodeToQueue(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "StartFor") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "IN PIPE" || connectedPlug.getName() == "StartFor") {
                    qDebug() << "adding" << connectedPlug.getNode().getName() << "\StartFor";
                    nodesQueue->enqueue(allNodesDict.find(connectedPlug.getNode()).value());
                    return true;
                } else {
                    error->appendToErrorMessage(QString("error at node "));
                    error->appendToErrorMessage(node->getName());
                    error->appendToErrorMessage(QString(". StartFor can only be connected to IN PIPE\n"));
                }
            }
        }
    }
    return false;
}

void Parser::parseForNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse FOR";
    addIndentationToLine();
    if (!indentationLevel->contains(node)) { // starting for pipeline
        if (!addForStartNodeToQueue(node, nodesQueue)) {
            return;
        }
        indentationLevel->insert(node);
    } else {
        indentationLevel->remove(node);
        addNextNodeToQueue(node, nodesQueue);
        qDebug() << "second match for same FOR. End parse";
        return;
    }
    bool correctNodeFound = false;
    script->append("for ");
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "Variable") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "GET") {
                    script->append(connectedPlug.getNode().getName());
                    script->append(" in ");
                    correctNodeFound = true;
                    break;
                }
                break;
            }
        }
    }
    if (!correctNodeFound) {
        error->appendToErrorMessage("no VARIABLE found as iterator in FOR node\n");
        return;
    }
    correctNodeFound = false;
    for (auto plug : plugs) { //done separately to maintain order of parsing
        if (plug.getName() == "Collection") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "GET") {
                    script->append(connectedPlug.getNode().getName());
                    script->append(":\n");
                    correctNodeFound = true;
                    break;
                }
                break;
            }
        }
    }
    if (!correctNodeFound) {
        error->appendToErrorMessage("no VARIABLE found as a collection for FOR node\n");
        return;
    }
    qDebug() << "end parse FOR";
}

void Parser::parseMainWindowNode(NodeCtrl *node, QQueue<NodeCtrl *> *nodesQueue) {
    qDebug() << "begin parse MAINWINDOW\n";
    bool hasResult = false;
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "Result") {
            hasResult = true;
            if (plug.getConnectedPlugs().size() == 0) { // if return value is being used,
                                                        //dont connect this node to pipeline
                if (!addNextNodeToQueue(node, nodesQueue)) {
                    error->appendToErrorMessage(QString("pipeline disconnected at MAIN WINDOW node\n"));
                }
            }
            break;
        }
    }
    if (!hasResult) {
        if (!addNextNodeToQueue(node, nodesQueue)) {
            error->appendToErrorMessage(QString("pipeline disconnected at MAIN WINDOW node\n"));
        }
    }
    addIndentationToLine();
    QString funcName = ((MainWindowNode*)node->getNodeModel())->getFuncName();
    script->append("MainWindow.");
    if (funcName == "loadFile") {
        parseMWLoadFile(node);
    } else if (funcName == "saveScreen") {
        parseMWSaveScreen(node);
    } else {
        script->append(funcName);
        script->append("()");
    }
    script->append("\n");
    qDebug() << "end parse MAIN WINDOW";
}

void Parser::parseMWLoadFile(NodeCtrl *node) {
    script->append(QString("loadFile("));

    bool isFileNamePresent = false;
    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "fileName") {
            isFileNamePresent = true;
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "GET" || connectedPlug.getName() == "My Value") {
                    qDebug() << "adding fileName LOadFile";
                    script->append(connectedPlug.getNode().getName());
                    break;
                }
            }
        }
    }
    if (!isFileNamePresent) {
        error->appendToErrorMessage(QString("fileName is needed for MainWindow.LoadFile node"));
    }
    script->append(")");
}

void Parser::parseMWSaveScreen(NodeCtrl *node) {
    script->append(QString("saveScreen("));

    auto plugs = node->getPlugHandles();
    for (auto plug : plugs) {
        if (plug.getName() == "fileName") {
            for (auto connectedPlug : plug.getConnectedPlugs()) {
                if (connectedPlug.getName() == "GET" || connectedPlug.getName() == "My Value") {
                    qDebug() << "adding fileName SaveScreen";
                    script->append(connectedPlug.getNode().getName());
                    break;
                }
            }
        }
    }
    script->append(")");
}
