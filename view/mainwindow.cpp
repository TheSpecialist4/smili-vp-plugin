#include "view/mainwindow.h"

#include <QApplication>
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QStyleFactory>
#include <QToolBar>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QLabel>
#include <QComboBox>
#include <QScrollArea>
#include <QDebug>
#include <QFileDialog>
#include <QFile>

#include "nodectrl.h"
#include "mainctrl.h"
#include "propertyeditor.h"
#include "zodiacgraph/baseedge.h"
#include "zodiacgraph/edgearrow.h"
#include "zodiacgraph/edgelabel.h"
#include "zodiacgraph/node.h"
#include "zodiacgraph/nodelabel.h"
#include "zodiacgraph/perimeter.h"
#include "zodiacgraph/plug.h"
#include "zodiacgraph/pluglabel.h"
#include "zodiacgraph/scene.h"
#include "zodiacgraph/view.h"

#include "node/fornode.h"
#include "node/imageoperationnode.h"
#include "node/startnode.h"
#include "node/variablenode.h"
#include "node/valuenode.h"
#include "node/pythonnode.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //resize(1100,700);
    setWindowTitle("Visual Programming SMILI");

    isNewNodePanelOpen = false;

    scriptText = new QString();

    // create the main toolbar
    QToolBar* mainToolBar = new QToolBar(this);
    mainToolBar->setStyleSheet("QToolBar {border: 0px;}");
    mainToolBar->setIconSize(QSize(15,15));
    mainToolBar->setMovable(false);
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addToolBar(Qt::TopToolBarArea, mainToolBar);

    // create the status bar
    statusBar();

    // create the Zodiac graph
    zodiac::Scene* zodiacScene = new zodiac::Scene(this);
    zodiac::View* zodiacView = new zodiac::View(this);
    zodiacView->setScene(zodiacScene);

    // create the Property Editor
    PropertyEditor* propertyEditor = new PropertyEditor(this);

    nodePropertyTab = new QTabWidget;

    nodePropertyTab->addTab(propertyEditor, QIcon(":/icons/editicon.png"), tr("&Edit"));

    QLabel* templateHeading = new QLabel;
    templateHeading->setText(tr("Templates"));

    QPushButton* screenshotBtn = new QPushButton;
    screenshotBtn->setText(tr("Screenshot"));
    screenshotBtn->setMinimumHeight(50);

    QVBoxLayout* tempBox = new QVBoxLayout;
    tempBox->setContentsMargins(QMargins(4,0,4,0));
    tempBox->addWidget(templateHeading);
    tempBox->addWidget(screenshotBtn);
    tempBox->addStretch();

    QWidget* holder = new QWidget;
    holder->setLayout(tempBox);

    nodePropertyTab->addTab(holder, QIcon(":/icons/menu.png"),tr("&Templates"));

    // grid on left side of main splitter
    QGridLayout* leftGrid = new QGridLayout;
    createNewNodePanel(leftGrid);
    leftGrid->addWidget(nodePropertyTab);

    QWidget* leftGridWidget = new QWidget;
    leftGridWidget->setLayout(leftGrid);


    // create the Main Controller
    m_mainCtrl = new MainCtrl(this, zodiacScene, propertyEditor);
    //create default start and end nodes
    m_mainCtrl->createNode(QString("START"))->getNodeHandle().setPos(-100, -100);
    m_mainCtrl->createNode(QString("END"))->getNodeHandle().setPos(-100, 200);

    connect(screenshotBtn, SIGNAL(clicked(bool)), m_mainCtrl, SLOT(createScreenshotScript()));

    // debug output area
    scriptArea = new QTextEdit(this);
    scriptArea->setStyleSheet("QTextEdit {background: #D6D5DB; color: #000000}");
    scriptArea->setStatusTip(tr("Message Log"));
    scriptArea->setReadOnly(true);

    QSplitter* canvasSplitter = new QSplitter(Qt::Vertical, this);
    canvasSplitter->addWidget(zodiacView);
    canvasSplitter->addWidget(scriptArea);
    canvasSplitter->setSizes({900, 100});

    // setup the main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->addWidget(leftGridWidget);
    m_mainSplitter->addWidget(canvasSplitter);
    m_mainSplitter->setSizes({198, 802});

    // create global actions
    QAction* newNodeAction = new QAction(QIcon(":/icons/plus.svg"), tr("&Add Node"), this);
    newNodeAction->setShortcuts(QKeySequence::New);
    newNodeAction->setStatusTip(tr("Create a new Node"));
    mainToolBar->addAction(newNodeAction);
    //connect(newNodeAction, SIGNAL(triggered()), m_mainCtrl, SLOT(createDefaultNode()));
    connect(newNodeAction, SIGNAL(triggered()), this, SLOT(toggleNewNodePanelVisibility()));

    QAction* generateScriptAction = new QAction(QIcon(":/icons/gear.png"), tr("&Generate Script"), this);
    generateScriptAction->setStatusTip(tr("Generate Script"));
    mainToolBar->addAction(generateScriptAction);
    connect(generateScriptAction, SIGNAL(triggered()), this, SLOT(savePythonScript()));

    QWidget* emptySpacer = new QWidget();
    emptySpacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    mainToolBar->addWidget(emptySpacer);

    QAction* aboutAction = new QAction(QIcon(":/icons/questionmark.svg"), tr("&About"), this);
    aboutAction->setStatusTip(tr("Show the about box"));
    mainToolBar->addAction(aboutAction);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(displayAbout()));

    // initialize the GUI
    setCentralWidget(m_mainSplitter);
    readSettings();
    writeSettings();
    zodiacScene->updateStyle();
    zodiacView->updateStyle();
}

void MainWindow::createNewNodePanel(QGridLayout* leftGrid)
{
    QVBoxLayout* newNodeVBoxPanel = new QVBoxLayout;

    QFont* font = new QFont;
    font->setPointSize(9);
    int buttonHeight = 40;

    //------START---------------------------------
    QPushButton* btnStart = new QPushButton;
    btnStart->setText("START");
    btnStart->setMinimumHeight(buttonHeight);
    btnStart->setFont(*font);

    //-------Variable-----------------------------
    QPushButton* btnVariable = new QPushButton;
    btnVariable->setText("Variable");
    btnVariable->setMinimumHeight(buttonHeight);
    btnVariable->setFont(*font);

    //------For----------------------------------------
    QPushButton* btnFor = new QPushButton;
    btnFor->setText("For");
    btnFor->setMinimumHeight(buttonHeight);
    btnFor->setFont(*font);

    //--------If-----------------------------------------
    QPushButton* btnIf = new QPushButton;
    btnIf->setText("If/Else");
    btnIf->setMinimumHeight(buttonHeight);
    btnIf->setFont(*font);

    //--------ImageOperation------------------------------
    QVBoxLayout* imageOpLayout = new QVBoxLayout;
    QLabel* labelImageOp = new QLabel(QString("Image Operation"));
    labelImageOp->setFont(*font);

    QComboBox* comboImageOp = new QComboBox;
    comboImageOp->setMinimumHeight(30);
    comboImageOp->setFont(*font);
    QStringList imageOps;
    imageOps << "ImageOp.vectorField" << "ImageOp.pseudoImage" << "ImageOp.streamLines"
             << "ImageOp.median" << "ImageOp.scale";
    comboImageOp->addItems(imageOps);

    connect(comboImageOp, SIGNAL(activated(QString)), this, SLOT(createNewNode(QString)));

    imageOpLayout->addWidget(labelImageOp);
    imageOpLayout->addWidget(comboImageOp);

    QWidget* imageOpHolder = new QWidget;
    imageOpHolder->setLayout(imageOpLayout);
    imageOpHolder->setMaximumHeight(100);
    imageOpHolder->setStyleSheet(QString(".QWidget{border: 1px solid #383a3f;background: #2d2f33}"));

    //--------ModelOperation-------------------------------
    QVBoxLayout* modelOpLayout = new QVBoxLayout;
    QLabel* labelModelOp = new QLabel(QString("Model Operation"));
    labelModelOp->setFont(*font);

    QComboBox* comboModelOp = new QComboBox;
    comboModelOp->setMinimumHeight(30);
    comboModelOp->setFont(*font);
    comboModelOp->addItem(QString("ModelOp.loadView"));
    comboModelOp->addItem(QString("ModelOp.clean"));
    comboModelOp->addItem(QString("ModelOp.smooth"));

    connect(comboModelOp, SIGNAL(activated(QString)), this, SLOT(createNewNode(QString)));

    modelOpLayout->addWidget(labelModelOp);
    modelOpLayout->addWidget(comboModelOp);

    QWidget* modelOpHolder = new QWidget;
    modelOpHolder->setLayout(modelOpLayout);
    modelOpHolder->setMaximumHeight(100);
    modelOpHolder->setStyleSheet(QString(".QWidget{border: 1px solid #383a3f;background: #2d2f33}"));

    //---------MainWindow-----------------------------------
    QVBoxLayout* mainWindowLayout = new QVBoxLayout;
    QLabel* labelMW = new QLabel(QString("Main Window"));
    labelMW->setFont(*font);

    QComboBox* comboMW = new QComboBox;
    comboMW->setMinimumHeight(30);
    comboMW->setFont(*font);
    comboMW->addItem(QString("MainWindow.loadFile"));
    comboMW->addItem(QString("MainWindow.activeImage"));
    comboMW->addItem(QString("MainWindow.activeModel"));
    comboMW->addItem(QString("MainWindow.tileTab"));
    comboMW->addItem(QString("MainWindow.saveScreen"));

    connect(comboMW, SIGNAL(activated(QString)), this, SLOT(createNewNode(QString)));

    mainWindowLayout->addWidget(labelMW);
    mainWindowLayout->addWidget(comboMW);

    QWidget* mwWidget = new QWidget;
    mwWidget->setLayout(mainWindowLayout);
    mwWidget->setMaximumHeight(100);
    mwWidget->setStyleSheet(QString(".QWidget{border: 1px solid #383a3f;background: #2d2f33}"));

    //------Value------------------------------------
    QPushButton* btnValue = new QPushButton;
    btnValue->setText("Value");
    btnValue->setMinimumHeight(buttonHeight);
    btnValue->setFont(*font);

    //----------python-------------------------------
    QVBoxLayout* pythonLayout = new QVBoxLayout;
    QLabel* labelPython = new QLabel(QString("Python"));
    labelPython->setFont(*font);

    QComboBox* comboPython = new QComboBox;
    comboPython->setMinimumHeight(30);
    comboPython->setFont(*font);
    comboPython->addItem(QString("Python.print"));
    //comboPython->addItem(QString("Python.execfile"));

    connect(comboPython, SIGNAL(activated(QString)), this, SLOT(createNewNode(QString)));

    pythonLayout->addWidget(labelPython);
    pythonLayout->addWidget(comboPython);

    QWidget* pythonHolder = new QWidget;
    pythonHolder->setLayout(pythonLayout);
    pythonHolder->setStyleSheet(QString(".QWidget{border: 1px solid #383a3f;background: #2d2f33}"));
    pythonHolder->setMaximumHeight(100);

    //------END------------------------------------------------------
    QPushButton* btnEnd = new QPushButton;
    btnEnd->setText("END");
    btnEnd->setMinimumHeight(buttonHeight);
    btnEnd->setFont(*font);

    //----SignalMapper-----------------------------------------------
    QSignalMapper* signalMapper = new QSignalMapper(this);

    connect(btnStart, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnStart, QString("START"));

    connect(btnVariable, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnVariable, QString("Variable"));

    connect(btnFor, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnFor, QString("For"));

    connect(btnIf, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnIf, QString("If"));

    connect(btnValue, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnValue, QString("Value"));

    connect(btnEnd, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnEnd, QString("END"));

    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(createNewNode(QString)));

    newNodeVBoxPanel->addWidget(btnStart);
    newNodeVBoxPanel->addWidget(btnVariable);
    newNodeVBoxPanel->addWidget(btnFor);
    newNodeVBoxPanel->addWidget(btnIf);
    newNodeVBoxPanel->addWidget(imageOpHolder);
    newNodeVBoxPanel->addWidget(modelOpHolder);
    newNodeVBoxPanel->addWidget(mwWidget);
    newNodeVBoxPanel->addWidget(btnValue);
    newNodeVBoxPanel->addWidget(pythonHolder);
    newNodeVBoxPanel->addWidget(btnEnd);

    QWidget* newNodeLayoutHolder = new QWidget;
    newNodeLayoutHolder->setLayout(newNodeVBoxPanel);
    newNodeLayoutHolder->setMinimumWidth(150);

    newNodePanel = new QScrollArea;
    newNodePanel->setWidget(newNodeLayoutHolder);

    leftGrid->addWidget(newNodePanel);
    newNodePanel->hide();
}

void MainWindow::createNewNode(QString nodeName) {
    isNewNodePanelOpen = false;
    nodePropertyTab->show();
    newNodePanel->hide();

    m_mainCtrl->createNode(nodeName);
}

void MainWindow::appendToScriptArea(QString& text) {
    scriptText->append(text);
    scriptArea->setText(*scriptText);
}

void MainWindow::toggleNewNodePanelVisibility()
{
    isNewNodePanelOpen = !isNewNodePanelOpen;
    // hide current selected node properties
    (!isNewNodePanelOpen) ? nodePropertyTab->show() : nodePropertyTab->hide();
    // show new node panel
    (isNewNodePanelOpen) ? newNodePanel->show() : newNodePanel->hide();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_mainCtrl->shutdown()) {
        // if the user accepted, close the application
        writeSettings();
        event->accept();

    } else {
        // if the user objected, do nothing
        event->ignore();
    }
}

void MainWindow::displayAbout()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("About this application");
    aboutBox.setText(
        "<h3>Visual Programming Interface for SMILI</h3>"
        "This application allows you create python scripts for SMILI <a href=\"smili-project.sourceforge.net\"</a>"
        "using graphical nodes."
         );
    aboutBox.exec();
}

void MainWindow::displayScript()
{
    m_mainCtrl->saveScript();
}

void MainWindow::savePythonScript() {
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Python File"), "", tr("Python (*.py);;All Files(*)"));
    if (fileName.isEmpty()) {
        return;
    } else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }
        QTextStream out(&file);
        out << m_mainCtrl->generateScript();

        QString messageLog = "";

        if (m_mainCtrl->getScriptErrorHandler()->isErrorPresent()) {
            messageLog.append(m_mainCtrl->getScriptErrorHandler()->getErrorMessage());
            messageLog.append("---------------------------------\n");
        } else {
            messageLog.append("The python script was generated!");
        }

        scriptArea->setText(messageLog);
    }
}

void MainWindow::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

    // apply custom palette and style
    QPalette palette;
    settings.beginGroup("palette");
    palette.setColor(QPalette::Window, settings.value("window", "#353535").toString());
    palette.setColor(QPalette::WindowText, settings.value("windowText", "#E0E2E4").toString());
    palette.setColor(QPalette::Base, settings.value("base", "#191919").toString());
    palette.setColor(QPalette::AlternateBase, settings.value("alternateBase", "#353535").toString());
    palette.setColor(QPalette::ToolTipBase, settings.value("toolTipBase", "#000000").toString());
    palette.setColor(QPalette::ToolTipBase, settings.value("toolTipBase", "#292C2E").toString());
    palette.setColor(QPalette::ToolTipText, settings.value("toolTipText", "#808080").toString());
    palette.setColor(QPalette::Text, settings.value("text", "#E0E2E4").toString());
    palette.setColor(QPalette::Button, settings.value("button", "#353535").toString());
    palette.setColor(QPalette::ButtonText, settings.value("buttonText", "#E0E2E4").toString());
    palette.setColor(QPalette::BrightText, settings.value("brightText", "#ffffff").toString());
    palette.setColor(QPalette::Link, settings.value("link", "#2a82da").toString());
    palette.setColor(QPalette::Highlight, settings.value("highlight", "#2a82da").toString());
    palette.setColor(QPalette::HighlightedText, settings.value("highlightedText", "#000000").toString());
    settings.endGroup();
    qApp->setPalette(palette);
    qApp->setStyle(QStyleFactory::create(settings.value("style", QString("Fusion")).toString()));

    // apply window position and size
    settings.beginGroup("mainWindow");
    resize(settings.value("size", QSize(1080, 600)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    m_mainSplitter->restoreState(settings.value("propertySplitter").toByteArray());
    settings.endGroup();

    // apply ZodiacGraph settings
    settings.beginGroup("zodiac");

    settings.beginGroup("baseEdge");
    zodiac::BaseEdge::setBaseWidth(settings.value("width", 2.5).toReal());
    zodiac::BaseEdge::setBaseColor(settings.value("color", "#cc5d4e").toString());
    settings.endGroup();

    settings.beginGroup("edgeArrow");
    zodiac::EdgeArrow::defineArrow(settings.value("length", 12).toReal(), settings.value("width", 15).toReal());
    zodiac::EdgeArrow::setArrowColor(settings.value("color", "#cc5d4e").toString());
    settings.endGroup();

    settings.beginGroup("edgeLabel");
    zodiac::EdgeLabel::setFontFamily(settings.value("fontFamily", "DejaVu Sans Mono").toString());
    zodiac::EdgeLabel::setPointSize(settings.value("pointSize", 10).toReal());
    zodiac::EdgeLabel::setWeight(QFont::Weight(settings.value("weight", 75).toInt()));
    zodiac::EdgeLabel::setColor(settings.value("color", "#c8c8c8").toString());
    zodiac::EdgeLabel::setTransparency(settings.value("transparency", 0.7).toReal());
    zodiac::EdgeLabel::setVerticalOffset(settings.value("verticalOffset", 0.5).toReal());
    settings.endGroup();

    settings.beginGroup("node");
    zodiac::Node::setCoreRadius(settings.value("coreRadius", 25).toReal());
    zodiac::Node::setIdleColor(settings.value("idleColor", "#4b77a7").toString());
    zodiac::Node::setSelectedColor(settings.value("selectedColor", "#62abfa").toString());
    zodiac::Node::setOutlineColor(settings.value("outlineColor", "#cdcdcd").toString());
    zodiac::Node::setOutlineWidth(settings.value("outlineWidth", 3).toReal());
    zodiac::Node::setPlugSweep(settings.value("plugSweep", 32.5).toReal());
    zodiac::Node::setGapSweep(settings.value("gapSweep", 8.125).toReal());
    settings.endGroup();

    settings.beginGroup("nodeLabel");
    zodiac::NodeLabel::setTextColor(settings.value("textColor", "#ffffff").toString());
    zodiac::NodeLabel::setBackgroundColor(settings.value("backgroundColor", "#426998").toString());
    zodiac::NodeLabel::setLineColor(settings.value("lineColor", "#cdcdcd").toString());
    zodiac::NodeLabel::setLineWidth(settings.value("lineWidth", 1.5).toReal());
    zodiac::NodeLabel::setCornerRadius(settings.value("cornerRadius", 8).toReal());
    zodiac::NodeLabel::setVerticalMargin(settings.value("verticalMargin", 2).toReal());
    zodiac::NodeLabel::setHorizontalMargin(settings.value("horizontalMargin", 4).toReal());
    zodiac::NodeLabel::setFontFamily(settings.value("fontFamily", "DejaVu Sans Mono").toString());
    zodiac::NodeLabel::setPointSize(settings.value("pointSize", 9).toReal());
    zodiac::NodeLabel::setWeight(QFont::Weight(settings.value("weight", 63).toInt()));
    settings.endGroup();

    settings.beginGroup("perimeter");
    zodiac::Perimeter::setColor(settings.value("color", "#2b517d").toString());
    settings.endGroup();

    settings.beginGroup("plug");
    zodiac::Plug::setWidth(settings.value("width", 12).toReal());
    zodiac::Plug::setInColor(settings.value("inColor", "#728872").toString());
    zodiac::Plug::setOutColor(settings.value("outColor", "#887272").toString());
    zodiac::Plug::setHighlightColor(settings.value("highlightColor", "#d1d7db").toString());
    settings.endGroup();

    settings.beginGroup("plugLabel");
    zodiac::PlugLabel::setFontFamily(settings.value("fontFamily", "DejaVu Sans Mono").toString());
    zodiac::PlugLabel::setPointSize(settings.value("pointSize", 10).toReal());
    zodiac::PlugLabel::setWeight(QFont::Weight(settings.value("weight", 75).toInt()));
    zodiac::PlugLabel::setColor(settings.value("color", "#828688").toString());
    zodiac::PlugLabel::setLabelDistance(settings.value("distance", 15).toReal());
    settings.endGroup();

    settings.beginGroup("view");
    zodiac::View::setBackgroundColor(settings.value("backgroundColor", "#191919").toString());
    //zodiac::View::setBackgroundColor(settings.value("backgroundColor", "#ffffff").toString());
    settings.endGroup();

    settings.endGroup(); // zodiac
}

void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());

    // write out current palette
    QPalette palette = qApp->palette();
    settings.beginGroup("palette");
    settings.setValue("window", palette.color(QPalette::Window).name());
    settings.setValue("windowText", palette.color(QPalette::WindowText).name());
    settings.setValue("base", palette.color(QPalette::Base).name());
    settings.setValue("alternateBase", palette.color(QPalette::AlternateBase).name());
    settings.setValue("toolTipBase", palette.color(QPalette::ToolTipBase).name());
    settings.setValue("toolTipText", palette.color(QPalette::ToolTipText).name());
    settings.setValue("text", palette.color(QPalette::Text).name());
    settings.setValue("button", palette.color(QPalette::Button).name());
    settings.setValue("buttonText", palette.color(QPalette::ButtonText).name());
    settings.setValue("brightText", palette.color(QPalette::BrightText).name());
    settings.setValue("link", palette.color(QPalette::Link).name());
    settings.setValue("highlight", palette.color(QPalette::Highlight).name());
    settings.setValue("highlightedText", palette.color(QPalette::HighlightedText).name());
    settings.endGroup();

    // write out MainWindow position and size
    settings.beginGroup("mainWindow");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("propertySplitter", m_mainSplitter->saveState());
    settings.endGroup();

    // write out ZodiacGraph settings
    settings.beginGroup("zodiac");

    settings.beginGroup("baseEdge");
    settings.setValue("width", zodiac::BaseEdge::getBaseWidth());
    settings.setValue("color", zodiac::BaseEdge::getBaseColor().name());
    settings.endGroup();

    settings.beginGroup("edgeArrow");
    settings.setValue("width", zodiac::EdgeArrow::getArrowWidth());
    settings.setValue("length", zodiac::EdgeArrow::getArrowLength());
    settings.setValue("color", zodiac::EdgeArrow::getArrowColor().name());
    settings.endGroup();

    settings.beginGroup("edgeLabel");
    settings.setValue("fontFamily", zodiac::EdgeLabel::getFontFamily());
    settings.setValue("pointSize", zodiac::EdgeLabel::getPointSize());
    settings.setValue("weight", zodiac::EdgeLabel::getWeight());
    settings.setValue("color", zodiac::EdgeLabel::getColor().name());
    settings.setValue("transparency", zodiac::EdgeLabel::getTransparency());
    settings.setValue("verticalOffset", zodiac::EdgeLabel::getVerticalOffset());
    settings.endGroup();

    settings.beginGroup("node");
    settings.setValue("coreRadius", zodiac::Node::getCoreRadius());
    settings.setValue("idleColor", zodiac::Node::getIdleColor().name());
    settings.setValue("selectedColor", zodiac::Node::getSelectedColor().name());
    settings.setValue("outlineColor", zodiac::Node::getOutlineColor().name());
    settings.setValue("outlineWidth", zodiac::Node::getOutlineWidth());
    settings.setValue("plugSweep", zodiac::Node::getPlugSweep());
    settings.setValue("gapSweep", zodiac::Node::getGapSweep());
    settings.endGroup();

    settings.beginGroup("nodeLabel");
    settings.setValue("textColor", zodiac::NodeLabel::getTextColor().name());
    settings.setValue("backgroundColor", zodiac::NodeLabel::getBackgroundColor().name());
    settings.setValue("lineColor", zodiac::NodeLabel::getLineColor().name());
    settings.setValue("lineWidth", zodiac::NodeLabel::getLineWidth());
    settings.setValue("cornerRadius", zodiac::NodeLabel::getCornerRadius());
    settings.setValue("verticalMargin", zodiac::NodeLabel::getVerticalMargin());
    settings.setValue("horizontalMargin", zodiac::NodeLabel::getHorizontalMargin());
    settings.setValue("fontFamily", zodiac::NodeLabel::getFontFamily());
    settings.setValue("pointSize", zodiac::NodeLabel::getPointSize());
    settings.setValue("weight", zodiac::NodeLabel::getWeight());
    settings.endGroup();

    settings.beginGroup("perimeter");
    settings.setValue("color", zodiac::Perimeter::getColor().name());
    settings.endGroup();

    settings.beginGroup("plug");
    settings.setValue("width", zodiac::Plug::getWidth());
    settings.setValue("inColor", zodiac::Plug::getInColor().name());
    settings.setValue("outColor", zodiac::Plug::getOutColor().name());
    settings.setValue("highlightColor", zodiac::Plug::getHighlightColor().name());
    settings.endGroup();

    settings.beginGroup("plugLabel");
    settings.setValue("fontFamily", zodiac::PlugLabel::getFontFamily());
    settings.setValue("pointSize", zodiac::PlugLabel::getPointSize());
    settings.setValue("weight", zodiac::PlugLabel::getWeight());
    settings.setValue("color", zodiac::PlugLabel::getColor().name());
    settings.setValue("distance", zodiac::PlugLabel::getLabelDistance());
    settings.endGroup();

    settings.beginGroup("view");
    settings.setValue("backgroundColor", zodiac::View::getBackgroundColor().name());
    settings.endGroup();

    settings.endGroup(); // zodiac
}
