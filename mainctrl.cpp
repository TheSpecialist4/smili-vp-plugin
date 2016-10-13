#include "mainctrl.h"

#include <QDebug>
#include <cstdlib>

#include "nodectrl.h"
#include "propertyeditor.h"
#include "zodiacgraph/nodehandle.h"

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

NodeCtrl* MainCtrl::createNode(const QString& name)
{
    // the newly created Node is the only selected one to avoid confusion
    m_scene.deselectAll();

    // use the given name or construct a default one
    QString nodeName = name;
    if(nodeName.isEmpty()){
        nodeName = s_defaultName + QString::number(m_nodeIndex++);
    }

    // create the node
    NodeCtrl* nodeCtrl = new NodeCtrl(this, m_scene.createNode(nodeName));
    m_nodes.insert(nodeCtrl->getNodeHandle(), nodeCtrl);

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

//    if(!node->isRemovable()){
//        // nodes with connections cannot be deleted
//        return false;
//    }

    // disconnect and delete the node
    node->disconnect();
    zodiac::NodeHandle handle = node->getNodeHandle();
    m_nodes.remove(handle);
    bool result = handle.remove();
    //Q_ASSERT(result);
    return result;
}

void MainCtrl::printZodiacScene()
{
    QList<zodiac::NodeHandle> allNodes = m_nodes.keys();
    for(zodiac::NodeHandle node : allNodes){
        int number = node.getName().right(2).trimmed().toInt();
        QString nodeCtrl = "nodeCtrl" + QString::number(number);
        QPointF pos = node.getPos();

        qDebug() << "NodeCtrl* nodeCtrl" + QString::number(number) + " = mainCtrl->createNode(\"" + node.getName() + "\");";
        qDebug() << nodeCtrl + "->getNodeHandle().setPos(" + QString::number(pos.x()) + ", " + QString::number(pos.y()) + ");";

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
                qDebug() << nodeCtrl + "->getNodeHandle().getPlug(\"" + plug.getName() + "\").connectPlug(" + otherNodeCtrl + "->getNodeHandle().getPlug(\"" + otherPlug.getName() + "\"));";
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


void MainCtrl::createDefaultNode()
{
    NodeCtrl* newNode = createNode();

//    int plugCount = (qreal(qrand())/qreal(RAND_MAX))*12;
//    for(int i = 0; i < plugCount + 4; ++i){
//        if((qreal(qrand())/qreal(RAND_MAX))<0.5){
//            newNode->addIncomingPlug("plug");
//        } else {
//            newNode->addOutgoingPlug("plug");
//        }
//    }

    newNode->setSelected(true);
}

QString MainCtrl::getScript()
{
    QString script = "Script: ";
    for (auto key : m_nodes.keys()) {
        script.append(m_nodes[key]->getName());
        script.append(" ");
    }
    qDebug() << script;
    return script;
}

void MainCtrl::createScreenshotScript()
{
    for (auto node : m_nodes.keys()) {
        this->deleteNode(m_nodes[node]);
    }

    NodeCtrl* var1 = this->createNode("\"RegOut_7.nii.gz\"");
    var1->getNodeHandle().setPos(-500, -680);
    var1->addOutgoingPlug("value");

    NodeCtrl* loadFile = this->createNode("LoadFile");
    loadFile->getNodeHandle().setPos(-500, -480);
    loadFile->addIncomingPlug("fileName");
    loadFile->addOutgoingPlug("file");

    NodeCtrl* loadFile2 = this->createNode("LoadFile");
    loadFile2->getNodeHandle().setPos(-100, -480);
    loadFile2->addIncomingPlug("fileName");
    loadFile2->addOutgoingPlug("file");

    NodeCtrl* var2 = this->createNode("\"RegDOut_7.nii.gz\"");
    var2->getNodeHandle().setPos(-100, -680);
    var2->addOutgoingPlug("value");

    NodeCtrl* vectorField = this->createNode("VectorField");
    vectorField->addIncomingPlug("file");
    vectorField->addIncomingPlug("subSampleFactor");
    vectorField->addIncomingPlug("scaling");
    vectorField->getNodeHandle().setPos(-100, -600);

    NodeCtrl* varSubSample = this->createNode("8");
    varSubSample->getNodeHandle().setPos(-10, -600);
    varSubSample->addOutgoingPlug("value");

    NodeCtrl* varScale = this->createNode("3.0");
    varScale->getNodeHandle().setPos(-10, -550);
    varScale->addOutgoingPlug("value");

    NodeCtrl* camera = this->createNode("CameraView");
    camera->getNodeHandle().setPos(-400, -200);
    camera->addIncomingPlug("fileName");
    camera->addOutgoingPlug("file");

    NodeCtrl* varCam = this->createNode("\"camera.cam\"");
    varCam->getNodeHandle().setPos(-450, -230);
    varCam->addOutgoingPlug("value");

    NodeCtrl* importView = this->createNode("ImportView");
    importView->getNodeHandle().setPos(-500, -400);
    importView->addIncomingPlug("image");
    importView->addOutgoingPlug("view");

    NodeCtrl* screenshot = this->createNode("Screenshot");
    screenshot->getNodeHandle().setPos(-50, 0);
    screenshot->addIncomingPlug("image1");
    screenshot->addIncomingPlug("image2");
    screenshot->addIncomingPlug("camera");
    screenshot->addOutgoingPlug("outputFile");

    NodeCtrl* out = this->createNode("OpenFile");
    out->getNodeHandle().setPos(0, 30);
    out->addIncomingPlug("inputStream");
    out->addOutgoingPlug("outputFile");

    NodeCtrl* varOut = this->createNode("\"screenie.png\"");
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
}

void MainCtrl::selectionChanged(QList<zodiac::NodeHandle> selection)
{
    m_propertyEditor->showNodes(selection);
}
