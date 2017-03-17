#include "mainwindow.h"

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Visual Programming SMILI");
    //setWindowIcon(QIcon(":/icons/zodiac_logo.png"));

    isNewNodePanelOpen = false;

    // create the main toolbar
    QToolBar* mainToolBar = new QToolBar(this);
    //mainToolBar->setFixedHeight(50);
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
    //nodePropertyTab->setTabPosition(QnodePropertyTab::West);

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

    //tree view to add node
//    tempButton = new QPushButton;
//    tempButton->setText(tr("Temporary button"));
//    tempButton->hide();
//    tempButton->setMaximumHeight(100);


    // grid on left side of main splitter
    QGridLayout* leftGrid = new QGridLayout;
    createNewNodePanel(leftGrid);
    leftGrid->addWidget(nodePropertyTab);

    QWidget* leftGridWidget = new QWidget;
    leftGridWidget->setLayout(leftGrid);


    // create the Main Controller
    m_mainCtrl = new MainCtrl(this, zodiacScene, propertyEditor);

    connect(screenshotBtn, SIGNAL(clicked(bool)), m_mainCtrl, SLOT(createScreenshotScript()));

    //QHBoxLayout* scriptBox = new QHBoxLayout(this);
    scriptArea = new QTextEdit(this);
    scriptArea->setStyleSheet("QTextEdit {background: #D6D5DB; color: #000000}");
    scriptArea->setStatusTip(tr("Python script generated"));
    //scriptArea->setReadOnly(true);
    //scriptBox->addWidget(scriptArea, 1);

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
    connect(generateScriptAction, SIGNAL(triggered()), this, SLOT(displayScript()));

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
    zodiacScene->updateStyle();
    zodiacView->updateStyle();

    //createZodiacLogo(m_mainCtrl);
}

void MainWindow::createNewNodePanel(QGridLayout* leftGrid)
{
    QVBoxLayout* newNodeVBoxPanel = new QVBoxLayout;

    QFont* font = new QFont;
    font->setPointSize(10);

    QPushButton* btnVariable = new QPushButton;
    btnVariable->setText("Variable");
    btnVariable->setMinimumHeight(50);
    btnVariable->setFont(*font);

    QPushButton* btnFor = new QPushButton;
    btnFor->setText("For");
    btnFor->setMinimumHeight(50);
    btnFor->setFont(*font);

    QPushButton* btnIf = new QPushButton;
    btnIf->setText("If/Else");
    btnIf->setMinimumHeight(50);
    btnIf->setFont(*font);

    QPushButton* btnMainWindow = new QPushButton;
    btnMainWindow->setText("Main Window");
    btnMainWindow->setMinimumHeight(50);
    btnMainWindow->setFont(*font);

    QPushButton* btnIdentifier = new QPushButton;
    btnIdentifier->setText("Identifier");
    btnIdentifier->setMinimumHeight(50);
    btnIdentifier->setFont(*font);

    QPushButton* btnPython = new QPushButton;
    btnPython->setText("Python Inbuilt");
    btnPython->setMinimumHeight(50);
    btnPython->setFont(*font);

    QSignalMapper* signalMapper = new QSignalMapper(this);

    connect(btnVariable, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnVariable, QString("Variable"));

    connect(btnFor, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnFor, QString("For"));

    connect(btnIf, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnIf, QString("If"));

    connect(btnMainWindow, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnMainWindow, QString("Main Window"));

    connect(btnIdentifier, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnIdentifier, QString("Identifier"));

    connect(btnPython, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(btnPython, QString("Python"));

    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(createNewNode(QString)));

    newNodeVBoxPanel->addWidget(btnVariable);
    newNodeVBoxPanel->addWidget(btnFor);
    newNodeVBoxPanel->addWidget(btnIf);
    newNodeVBoxPanel->addWidget(btnMainWindow);
    newNodeVBoxPanel->addWidget(btnIdentifier);
    newNodeVBoxPanel->addWidget(btnPython);

    newNodeLayoutHolder = new QWidget;
    newNodeLayoutHolder->setLayout(newNodeVBoxPanel);

    leftGrid->addWidget(newNodeLayoutHolder);

    newNodeLayoutHolder->hide();

}

void MainWindow::createNewNode(QString nodeName) {
    isNewNodePanelOpen = false;
    nodePropertyTab->show();
    newNodeLayoutHolder->hide();
    m_mainCtrl->createNode(nodeName);
}

//void MainWindow::hideNewNodePanel() {

//}

void MainWindow::toggleNewNodePanelVisibility()
{
    isNewNodePanelOpen = !isNewNodePanelOpen;
    // hide current selected node properties
    (!isNewNodePanelOpen) ? nodePropertyTab->show() : nodePropertyTab->hide();
    // show new node panel
    (isNewNodePanelOpen) ? newNodeLayoutHolder->show() : newNodeLayoutHolder->hide();
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
    aboutBox.setWindowTitle("About the plugin");
    aboutBox.setText(
        "<h3>About this Application</h3>"
        "This application allows you to generate python scripts for SMILI <a href=\"smili-project.sourceforge.net\"</a>"
        "using visual nodes."
         );
    aboutBox.exec();
}

void MainWindow::displayScript()
{
    //scriptArea->setText(m_mainCtrl->getScript());
    m_mainCtrl->saveScript();
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
    //palette.setColor(QPalette::ToolTipBase, settings.value("toolTipBase", "#000000").toString());
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
