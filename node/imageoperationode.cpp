#include "imageoperationnode.h"

ImageOperationNode::ImageOperationNode(QString name, NodeType nodeType, QString funcName)
    : NodeBase(name, nodeType)
{
    this->funcName = funcName;

    this->addInPlug(QString("Image Variable"));

    if (funcName == "vectorField") {
        createVectorFieldPlugs();
    } else if (funcName == "scale") {
        createScalePlugs();
    } else if (funcName == "pseudoImage") {
        createPseudoImagePlugs();
    } else if (funcName == "median") {
        createMedianPlugs();
    } else if (funcName == "streamLines") {
        createStreamLinesPlugs();
    }
}

QString ImageOperationNode::getFuncName() {
    return this->funcName;
}

void ImageOperationNode::createMedianPlugs() {
}

void ImageOperationNode::createPseudoImagePlugs() {
}

void ImageOperationNode::createScalePlugs() {
    this->addInPlug(QString("Scaling Value"));
}

void ImageOperationNode::createStreamLinesPlugs() {
}

void ImageOperationNode::createVectorFieldPlugs() {
    this->addInPlug(QString("subSampleFactor"));
    this->addInPlug(QString("scaling"));
}
