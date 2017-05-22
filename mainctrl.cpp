#include "mainctrl.h"
#include "view/mainwindow.h"

#include <QDebug>
#include <cstdlib>

#include "nodectrl.h"
#include "view/propertyeditor.h"
#include "zodiacgraph/nodehandle.h"
#include "parser.h"
#include "errorchecker.h"

#include "node/fornode.h"
#include "node/imageoperationnode.h"
#include "node/startnode.h"
#include "node/variablenode.h"
#include "node/valuenode.h"
#include "node/pythonnode.h"
#include "node/endnode.h"

QString MainCtrl::s_defaultName = "Node ";

MainCtrl::MainCtrl(QObject *parent, zodiac::Scene* scene, PropertyEditor* propertyEditor)
    : QObject(parent)
    , m_scene(zodiac::SceneHandle(scene))
    , m_propertyEditor(propertyEditor)
    , m_nodes(QHash<zodiac::NodeHandle, NodeCtrl*>())
    , m_nodeIndex(1)            // name suffixes start at 1
{
    m_propertyEditor->setMainCtrl(this);

    connect(&m_scene, SIGNAL(selectionChanged(QList<zodiac::NodeHandle>)),
            this, SLOT(selectionChanged(QList<zodiac::NodeHandle>)));
}

//NodeCtrl* MainCtrl::createNode(QString &nodeType)
//{
//    // the newly created Node is the only selected one to avoid confusion
//    m_scene.deselectAll();

//    QString nodeName = nodeType;
//    NodeCtrl* nodeCtrl = new NodeCtrl(this, m_scene.createNode(nodeName));
//    m_nodes.insert(nodeCtrl->getNodeHandle(), nodeCtrl);
//    //MainWindow::hideNewNodePanel();

//    return nodeCtrl;
//}

NodeCtrl* MainCtrl::createNode(QString &nodeType) {
    // the newly created Node is the only selected one to avoid confusion
    m_scene.deselectAll();

    bool isPresent = false;

    QString nodeName = nodeType;
    NodeCtrl* nodeCtrl = new NodeCtrl(this, m_scene.createNode(nodeName));
    m_nodes.insert(nodeCtrl->getNodeHandle(), nodeCtrl);

    NodeBase* node;

    if (nodeName == "Variable") {
        node = new VariableNode("Variable", NodeType::VAR_NODE);
        isPresent = true;
    } else if (nodeName == "START") {
        node = new StartNode("START", NodeType::START_NODE);
        isPresent = true;
    } else if (nodeName == "END") {
        node = new EndNode("END", NodeType::END_NODE);
        isPresent = true;
    } else if (nodeName == "For") {
        node = new ForNode("For", NodeType::FOR_NODE);
        isPresent = true;
    } else if (nodeName.contains("ImageOp")) {
        QString funcName = nodeName.split('.').at(1);
        qDebug() << "funcName " << funcName;
        node = new ImageOperationNode(nodeName, NodeType::IMAGE_OP_NODE, funcName);
        isPresent = true;
    } else if (nodeName.contains("ModelOp")) {
    } else if (nodeName == "Value") {
        node = new ValueNode("Value", NodeType::VALUE_NODE);
        isPresent = true;
    } else if (nodeName.contains(QString("Python"))) {
        QString funcName = nodeName.split('.').at(1);
        node = new PythonNode(nodeName, NodeType::PYTHON_NODE, funcName);
        isPresent = true;
    } else if (nodeName.contains("MainWindow")) {

    }


    if (isPresent) {
        nodeCtrl->setNodeModel(node);
        nodeCtrl->createPlugs();
    }

    return nodeCtrl;
}

bool MainCtrl::deleteNode(NodeCtrl* node)
{
#ifdef QT_DEBUG
    Q_ASSERT(m_nodes.contains(node->getNodeHandle()));
#else
    if(!m_nodes.contains(node->getNodeHandle())){
        return false;
    }
#endif

    if(!node->isRemovable()){
        // nodes with connections cannot be deleted
        return false;
    }

    // disconnect and delete the node
    node->disconnect();
    zodiac::NodeHandle handle = node->getNodeHandle();
    m_nodes.remove(handle);
    bool result = handle.remove();
    Q_ASSERT(result);
    return result;
}

void MainCtrl::printZodiacScene()
{
    QList<zodiac::NodeHandle> allNodes = m_nodes.keys();
    for(zodiac::NodeHandle node : allNodes){
        int number = node.getName().right(2).trimmed().toInt();
        QString nodeCtrl = "nodeCtrl" + QString::number(number);
        QPointF pos = node.getPos();

        qDebug() << "NodeCtrl* nodeCtrl" + QString::number(number) +
                    " = mainCtrl->createNode(\"" + node.getName() + "\");";
        qDebug() << nodeCtrl + "->getNodeHandle().setPos(" +
                    QString::number(pos.x()) + ", " + QString::number(pos.y()) + ");";

        for(zodiac::PlugHandle plug : node.getPlugs()){
            if(plug.isIncoming()){
                qDebug() << nodeCtrl + "->addIncomingPlug(\"" + plug.getName() + "\");";
            } else {
                qDebug() << nodeCtrl + "->addOutgoingPlug(\"" + plug.getName() + "\");";
            }
        }

        qDebug() << ""; // newline
    }

    for(zodiac::NodeHandle node : allNodes){
        int number = node.getName().right(2).trimmed().toInt();
        QString nodeCtrl = "nodeCtrl" + QString::number(number);
        for(zodiac::PlugHandle plug : node.getPlugs()){
            if(plug.isIncoming()) continue;
            for(zodiac::PlugHandle otherPlug : plug.getConnectedPlugs()){
                int otherNumber = otherPlug.getNode().getName().right(2).trimmed().toInt();
                QString otherNodeCtrl = "nodeCtrl" + QString::number(otherNumber);
                qDebug() << nodeCtrl + "->getNodeHandle().getPlug(\"" + plug.getName()
                            + "\").connectPlug(" + otherNodeCtrl + "->getNodeHandle().getPlug(\""
                            + otherPlug.getName() + "\"));";
            }
        }
    }
}

bool MainCtrl::shutdown()
{
    // do not receive any more signals from the scene handle
    m_scene.disconnect();

    return true;
}

void MainCtrl::saveScript()
{
    QList<zodiac::NodeHandle> allNodes = m_nodes.keys();
    QList<zodiac::NodeHandle> ready;
    QList<zodiac::NodeHandle> processed;

    //qDebug() << QString(getScript());
    //qDebug() << "allNodes Size: " << allNodes.size();

    processNodes(allNodes, ready, processed);
}

QString printList(QList<zodiac::NodeHandle> nodes) {
    QString out = "";
    for (auto node : nodes) {
        out += node.getName() + " ";
    }
    return out;
}

void MainCtrl::processNodes(QList<zodiac::NodeHandle> allNodes,
                           QList<zodiac::NodeHandle> ready, QList<zodiac::NodeHandle> processed)
{
    if (allNodes.size() == 0 && ready.size() == 0) {
        Q_ASSERT(this->m_nodes.keys().size() == processed.size()); //always true
        Q_ASSERT(ready.size() == 0); // always true
        qDebug() << "Processed size: " << processed.size();
        qDebug() << "Ready Size: " << ready.size();
        return;
    }
    // first node or individual node not connected
    if (ready.size() == 0) {
        ready.append(allNodes[0]);
        allNodes.removeAt(0);
    }
    /*
     * ###################################
     *  Algorithm
     * ###################################
     *
     * save (ready[0]);
     * for (neighbor : ready[0].neighbors) {
     *   ready.append(neighbor);
     * }
     * processed.append(ready[0]);
     * ready.removeAt(0);
     *
     * ###################################
     */
    saveNode(ready[0]);
    for (auto plug : ready[0].getPlugs()) {
        for (auto connectedPlug : plug.getConnectedPlugs()) {
            auto nodeHandle = connectedPlug.getNode();
            if (!(processed.contains(nodeHandle) || ready.contains(nodeHandle))) {
                ready.append(nodeHandle);
                qDebug() << "allNodes: " << printList(allNodes);
                qDebug() << "Added to queue: " << nodeHandle.getName();
                allNodes.removeOne(nodeHandle);
                qDebug() << "allNodes: " << printList(allNodes);
                qDebug() << "ready: " << printList(ready);
            }
            qDebug() << "allNodes size: " << allNodes.size();
        }
    }
    processed.append(ready[0]);
    ready.removeFirst();
    qDebug() << "Final ready: " << printList(ready);
    qDebug() << "Final processed: " << printList(processed);
    qDebug() << "Final allNodes: " << printList(allNodes);

    processNodes(allNodes, ready, processed);
}

bool MainCtrl::saveNode(zodiac::NodeHandle node)
{
    qDebug() << "Saved node: " << node.getName();
    return true;
}

QString MainCtrl::generateScript()
{
    parser = new Parser(m_nodes);
    auto error = parser->parseGraph(m_nodes.values());
    qDebug() << "Errors:\n" << (error.getErrorMessage()) << "\n";
    qDebug() << "Script:\n" << (parser->getScript());
    return parser->getScript();
}

ErrorChecker* MainCtrl::getScriptErrorHandler() {
    return parser->getErrorHandler();
}

void MainCtrl::createScreenshotScript()
{
    for (auto node : m_nodes.keys()) {
        this->deleteNode(m_nodes[node]);
    }

    NodeCtrl* var1 = this->createNode(QString("\"RegOut_7.nii.gz\""));
    var1->getNodeHandle().setPos(-500, -680);
    var1->addOutgoingPlug("value");

    NodeCtrl* loadFile = this->createNode(QString("LoadFile"));
    loadFile->getNodeHandle().setPos(-500, -480);
    loadFile->addIncomingPlug("fileName");
    loadFile->addOutgoingPlug("file");

    NodeCtrl* loadFile2 = this->createNode(QString("LoadFile"));
    loadFile2->getNodeHandle().setPos(-100, -480);
    loadFile2->addIncomingPlug("fileName");
    loadFile2->addOutgoingPlug("file");

    NodeCtrl* var2 = this->createNode(QString("\"RegDOut_7.nii.gz\""));
    var2->getNodeHandle().setPos(-100, -680);
    var2->addOutgoingPlug("value");

    NodeCtrl* vectorField = this->createNode(QString("VectorField"));
    vectorField->addIncomingPlug("file");
    vectorField->addIncomingPlug("subSampleFactor");
    vectorField->addIncomingPlug("scaling");
    vectorField->getNodeHandle().setPos(-100, -600);

    NodeCtrl* varSubSample = this->createNode(QString("8"));
    varSubSample->getNodeHandle().setPos(-10, -600);
    varSubSample->addOutgoingPlug("value");

    NodeCtrl* varScale = this->createNode(QString("3.0"));
    varScale->getNodeHandle().setPos(-10, -550);
    varScale->addOutgoingPlug("value");

    NodeCtrl* camera = this->createNode(QString("CameraView"));
    camera->getNodeHandle().setPos(-400, -200);
    camera->addIncomingPlug("fileName");
    camera->addOutgoingPlug("file");

    NodeCtrl* varCam = this->createNode(QString("\"camera.cam\""));
    varCam->getNodeHandle().setPos(-450, -230);
    varCam->addOutgoingPlug("value");

    NodeCtrl* importView = this->createNode(QString("ImportView"));
    importView->getNodeHandle().setPos(-500, -400);
    importView->addIncomingPlug("image");
    importView->addOutgoingPlug("view");

    NodeCtrl* screenshot = this->createNode(QString("Screenshot"));
    screenshot->getNodeHandle().setPos(-50, 0);
    screenshot->addIncomingPlug("image1");
    screenshot->addIncomingPlug("image2");
    screenshot->addIncomingPlug("camera");
    screenshot->addOutgoingPlug("outputFile");

    NodeCtrl* out = this->createNode(QString("OpenFile"));
    out->getNodeHandle().setPos(0, 30);
    out->addIncomingPlug("inputStream");
    out->addOutgoingPlug("outputFile");

    NodeCtrl* varOut = this->createNode(QString("\"screenie.png\""));
    varOut->addIncomingPlug("value");
    varOut->getNodeHandle().setPos(25, 100);

    var1->getNodeHandle().getPlug("value").connectPlug(loadFile->getNodeHandle().getPlug("fileName"));

    var2->getNodeHandle().getPlug("value").connectPlug(loadFile2->getNodeHandle().getPlug("fileName"));

    loadFile->getNodeHandle().getPlug("file").connectPlug(importView->getNodeHandle().getPlug("image"));

    importView->getNodeHandle().getPlug("view").connectPlug(screenshot->getNodeHandle().getPlug("image1"));

    varCam->getNodeHandle().getPlug("value").connectPlug(camera->getNodeHandle().getPlug("fileName"));

    camera->getNodeHandle().getPlug("file").connectPlug(screenshot->getNodeHandle().getPlug("camera"));

    varSubSample->getNodeHandle().getPlug("value").connectPlug(vectorField->getNodeHandle().getPlug("subSampleFactor"));

    varScale->getNodeHandle().getPlug("value").connectPlug(vectorField->getNodeHandle().getPlug("scaling"));

    loadFile2->getNodeHandle().getPlug("file").connectPlug(vectorField->getNodeHandle().getPlug("file"));
    loadFile2->getNodeHandle().getPlug("file").connectPlug(screenshot->getNodeHandle().getPlug("image2"));

    screenshot->getNodeHandle().getPlug("outputFile").connectPlug(out->getNodeHandle().getPlug("inputStream"));

    out->getNodeHandle().getPlug("outputFile").connectPlug(varOut->getNodeHandle().getPlug("value"));

    saveScript();
}

void MainCtrl::selectionChanged(QList<zodiac::NodeHandle> selection)
{
    m_propertyEditor->showNodes(selection);
}
