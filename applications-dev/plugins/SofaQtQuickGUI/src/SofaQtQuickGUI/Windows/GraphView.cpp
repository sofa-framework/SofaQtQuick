/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - erik.pernaud
    - damien.marchal@univ-lille.fr
********************************************************************/
#include <QGridLayout>

#define NODE_EDITOR_SHARED
#include <nodes/FlowView>
#include <nodes/FlowViewStyle>
#include <nodes/FlowScene>
#include <nodes/DataModelRegistry>
#include <nodes/ConnectionStyle>
#include <nodes/Node>

#include <sofa/core/ObjectFactory.h>
#include <sofa/core/ComponentLibrary.h>

#include <QAbstractAnimation>
#include <QPropertyAnimation>

#include "GraphView.h"
#include <SofaQtQuickGUI/Models/SofaComponentGraphModel.h>
using sofaqtquick::models::SofaComponentGraphModel;

#include <SofaSimulationGraph/DAGNode.h>
using sofa::simulation::graph::DAGNode;

namespace sofaqtquick::views
{
using namespace sofa::helper;

using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::ConnectionStyle;
using QtNodes::FlowViewStyle;
using sofa::core::objectmodel::Base;

void MyMutationListener::onBeginAddObject(Node* parent, BaseObject* obj)
{
    SOFA_UNUSED(obj);
}

void MyMutationListener::onEndAddObject(Node* parent, BaseObject* obj)
{
    emit addObject(parent, obj);
}


static std::shared_ptr<DataModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<DataModelRegistry>();

    std::vector<sofa::core::ClassEntry::SPtr> results;
    sofa::core::ObjectFactory::getInstance()->getAllEntries(results);

    ret->registerModel<SofaComponentGraphModel>();
    //for (auto compo : results)
    //{
    //    std::cout << compo->className << std::endl;
    //    ret->registerModel<SofaComponentGraphModel>(QString::fromStdString(compo->className));
    //}


    /*
    We could have more models registered.
    All of them become items in the context meny of the scene.

    ret->registerModel<AnotherDataModel>();
    ret->registerModel<OneMoreDataModel>();

    */

    return ret;
}


static void setGraphStyle()
{
    FlowViewStyle::setStyle(
                R"(
                {
                "FlowViewStyle": {
                "BackgroundColor": [114, 114, 114],
                "FineGridColor": [110,110,110],
                "CoarseGridColor": [70,70,60]
                }
                }
                )");

    ConnectionStyle::setConnectionStyle(
                R"(
                {
                "ConnectionStyle": {
                "LineWidth": 3.0,
                "UseDataDefinedColors": true
                }
                }
                )");
}



///////////////////////////////////////// ProfilerChartView ///////////////////////////////////



GraphView::GraphView(QWidget *parent)
    : QDialog(parent)
    , m_scaleX(10)
    , m_scaleY(30)
    , m_posX(0)
    , m_posY(0)
{    
    setStyleSheet("background-color:#727272;");
    setWindowTitle("GraphView");
    setGraphStyle();
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::WindowContextHelpButtonHint;
    setWindowFlags(flags);

    m_graphScene = new FlowScene(registerDataModels());
    m_exceptions = { "RequiredPlugin", "VisualStyle", "DefaultVisualManagerLoop", "InteractiveCamera" };
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_graphView = new FlowView(m_graphScene, this);
    layout->addWidget(m_graphView);
    this->setLayout(layout);
    
    resize(1000, 800);

    connect(&m_mutationListener, &MyMutationListener::addObject, this, &GraphView::onAddObject);

    if(m_rootNode==nullptr)
        return;

    m_rootNode->addListener(&m_mutationListener);

    // start from parsing root node
    parseSimulationNode(m_rootNode, 0, 0);

    // then connect all data
    connectNodeData();

}

void GraphView::onAddObject(Node* parent, BaseObject* object)
{
    QtNodes::Node& addedNode = m_graphScene->createNode(
                std::make_unique<SofaComponentGraphModel>(object, debugNodeGraph));

    m_nodeToQUid[QString::fromStdString(object->getPathName())] = &addedNode;
    addedNode.setObjectName(QString::fromStdString(object->getName()));

    SofaComponentGraphModel* model = dynamic_cast<SofaComponentGraphModel*>(addedNode.nodeDataModel());
    model->setCaption(object->getName());

    /// SEt position in the node's frame
    if(  m_nodeToQUid.contains(QString::fromStdString(parent->getPathName())) )
    {
        /// Add the object at the right node layer...
        auto parentNode = m_nodeToQUid[QString::fromStdString(parent->getPathName())];


        QPointF parentPos = m_graphScene->getNodePosition(*parentNode);
        QPointF newPos {parentPos.x()+20, parentPos.y()};
        int i = parent->getChildren().size();
        if( i != 0){
            auto lastChild = parent->getChildren()[i-1];
            if(m_nodeToQUid.contains(QString::fromStdString(lastChild->getPathName())))
            {
                QtNodes::Node* lastnode = m_nodeToQUid[QString::fromStdString(lastChild->getPathName())];
                QPointF lastPos = m_graphScene->getNodePosition(*lastnode);
                QSizeF  lastSize = m_graphScene->getNodeSize(*lastnode);

                newPos = QPointF{ lastPos.x()+lastSize.width()+60, lastPos.y()};
            }
        }
        m_graphScene->setNodePosition(addedNode, newPos );
    }
    moveTo(object);
}

GraphView::~GraphView()
{
    std::cout << "SofaWindowDataGraph::~SofaWindowDataGraph()" << std::endl;
    clearNodeData();
    // todo check if m_graphView need to be deleted. Normally no as child of QtWidget RealGui.
    //delete m_graphView;
}

SofaBase* GraphView::getSelectedComponent()
{
    return new SofaBase(m_selectedComponent);
}


const std::string getPathName(Base *base)
{
    BaseNode* node = base->toBaseNode();
    if(node)
        return node->getPathName();

    BaseObject* object = base->toBaseObject();
    if(object)
        return object->getPathName();

    return "invalid object";
}

void GraphView::moveTo(Base* base)
{
    if( m_nodeToQUid.contains(QString::fromStdString(getPathName(base))))
    {
        auto& node = m_nodeToQUid[QString::fromStdString(getPathName(base))];

        static MoveTooAnimation animation([this](double factor, MoveTooAnimation* self){
            QPointF pos = (self->m_destPosition -self->m_sourcePosition)*factor+self->m_sourcePosition;
            self->m_currentPosition = pos;

            QVector2D d{self->m_destPosition-self->m_sourcePosition};
            double es= d.length()/6000;
            double rs = 1.0;
            double df=factor*M_PI-M_PI_2;
            df = ((1.0-cos(df))+0.5)/1.5;
            if(es<0.5){
                es = es;
            }else{
                es= 1.0;
            }
            rs = (df*es)+(1.0-es);
            m_graphView->resetTransform();
            m_graphView->centerOn(pos);
            m_graphView->scale(rs,rs);
        });

        animation.set(animation.m_currentPosition, m_graphScene->getNodePosition(*node));
        animation.start();
    }
}

void GraphView::setSelectedComponent(SofaBase* base)
{
    m_selectedComponent = base->rawBase();
    emit selectedComponentChanged();
}

SofaNode* GraphView::getRootNode()
{
    return new SofaNode(DAGNode::SPtr(dynamic_cast<DAGNode*>(m_rootNode->toBaseNode())));
}

void GraphView::setRootNode(SofaNode* node)
{
    resetNodeGraph(node->self());
    emit rootNodeChanged();
}

void GraphView::clearNodeData()
{
    if (m_graphScene != nullptr)
    {
        msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "clear before: " << m_graphScene->allNodes().size();
        //m_graphScene->clear();
        delete m_graphScene;
        m_graphScene = new FlowScene(registerDataModels());
        m_graphView->setScene(m_graphScene);

        msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "clear after: " << m_graphScene->allNodes().size();
    }
    m_posX = 0;
    m_posY = 0;
    m_nodeToQUid.clear();
}

void GraphView::resetNodeGraph(sofa::simulation::Node* scene)
{
    if(m_rootNode)
    {
        m_rootNode->removeListener(&m_mutationListener);
    }

    setWindowTitle("GraphView: " + QString::fromStdString(scene->getPathName()));
    m_rootNode = scene;
    clearNodeData();

    m_rootNode->addListener(&m_mutationListener);

    // start from parsing root node
    parseSimulationNode(m_rootNode, 0, 0);

    // then connect all data
    connectNodeData();
}

QVector2D GraphView::getViewPosition()
{
    return m_viewPosition;
}

void GraphView::setViewPosition(QVector2D viewPosition)
{
    QVector2D dx = viewPosition-m_viewPosition;
    m_viewPosition = viewPosition;

    m_graphView->translate(dx.x(), dx.y());
    std::cout << "TP" << m_graphView->pos().x() << " => " << m_graphView->pos().y() << std::endl;


    emit viewPositionChanged();
}

//std::cout << "ADD OBJECT" << bObject << std::endl;

//const std::string& name = bObject->getClassName() + " - " + bObject->getName();
//msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "addSimulationObject: " << name;

//QtNodes::Node& fromNode = m_graphScene->createNode(
//            std::make_unique<SofaComponentGraphModel>(bObject, debugNodeGraph));
//            fromNode.setObjectName(QString::fromStdString(bObject->getName()));


//SofaComponentGraphModel* model = dynamic_cast<SofaComponentGraphModel*>(fromNode.nodeDataModel());
//model->setCaption(name);

//auto& fromNgo = fromNode.nodeGraphicsObject();
//fromNgo.setPos(m_posX, m_posY);
//m_posX += name.length() * m_scaleX;

//return model->getNbrData();


void GraphView::showConnectedComponents(SofaBase* base)
{
    std::cout << " SHOW CONNECTED COMPONENT ..... " << base->getName().toStdString() << std::endl;
    moveTo(base->rawBase());
    //clearNodeData();
    //findConnectedComponents(base->rawBase());
    //emit rootNodeChanged();
}

void GraphView::findConnectedComponents(Base* base)
{
    if(base==nullptr)
        return;

    BaseNode* node = base->toBaseNode() ;
    BaseObject* object = base->toBaseObject();

    if(node!=nullptr)
        return;

    if( m_nodeToQUid.contains(QString::fromStdString(object->getPathName())) )
        return;

    QtNodes::Node& fromNode = m_graphScene->createNode(
                std::make_unique<SofaComponentGraphModel>(object, debugNodeGraph));

    m_nodeToQUid[QString::fromStdString(object->getPathName())] = &fromNode;
    fromNode.setObjectName(QString::fromStdString(base->getName()));

    SofaComponentGraphModel* model = dynamic_cast<SofaComponentGraphModel*>(fromNode.nodeDataModel());
    model->setCaption(object->getName());

    //auto& fromNgo = fromNode.nodeGraphicsObject();
    //fromNgo.setOpacity(0.5);

    /// Explore the links
    for(auto& link : base->getLinks())
    {
        std::cout << "EXPLOR ENEIGHTBOOR " << link->getName() << std::endl;
        auto linkedBase = link->getLinkedBase();
        findConnectedComponents(linkedBase);
    }

    for(auto& data : base->getDataFields())
    {

    }
}

QRectF GraphView::parseSimulationNode(sofa::simulation::Node* node, double posX, double posY)
{
    if(node==nullptr)
        return QRectF {};

    msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << m_posY << " ### Child Name: " << node->getName();
    // first parse the list BaseObject inside this node
    std::vector<sofa::core::objectmodel::BaseObject*> bObjects = node->getNodeObjects();

    /// We create the node at a given X, Y
    QtNodes::Node* nodeparent = addSimulationNode(node, posX, posY);
    /// We get the previously created node.
    QSizeF nodeSize = m_graphScene->getNodeSize(*nodeparent);
    QPointF nodePos = m_graphScene->getNodePosition(*nodeparent);
    QPointF currentPos = nodePos ;
    double x_spacing = 60; // 10.0;
    double y_spacing = 60; // 10.0;

    QRectF currentRect { nodePos, nodeSize };

    currentPos.setX( currentPos.x() + nodeSize.width() + x_spacing*2.0);

    /// Add all the node's childs.
    for (auto bObject : bObjects)
    {
        bool skip = false;
        for (auto except : m_exceptions)
        {
            if (except == bObject->getClassName())
            {
                msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "skip: " << except;
                skip = true;
                break;
            }
        }

        if (skip)
            continue;

        QtNodes::Node* nodeobject = addSimulationObject(bObject, currentPos.x(), currentPos.y() );

        /// We get the previously created node.
        QSizeF objectSize = m_graphScene->getNodeSize(*nodeobject);
        QPointF objectPos = m_graphScene->getNodePosition(*nodeobject);
        currentPos.setX( objectPos.x() + objectSize.width()  + x_spacing);
        currentRect = currentRect.united( QRectF { objectPos, objectSize });
    }

    // second move to child nodes
    for (auto simuNode : node->getChildren())
    {
        QPointF bl = currentRect.bottomLeft();
        QRectF rect = parseSimulationNode(dynamic_cast<sofa::simulation::Node*>(simuNode),
                                          bl.x()+nodeSize.width()
                                          , bl.y()+y_spacing);
        currentRect = currentRect.united(rect);
    }

    return currentRect;
}


QtNodes::Node* GraphView::addSimulationNode(sofa::core::objectmodel::BaseNode* node, double x, double y)
{
    const std::string& name = node->getClassName() + " - " + node->getName();
    msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "addSimulationObject: " << name;

    QtNodes::Node& fromNode = m_graphScene->createNode(
                std::make_unique<SofaComponentGraphModel>(node, debugNodeGraph));
    fromNode.setObjectName(QString::fromStdString(node->getName()));

    m_nodeToQUid[QString::fromStdString(node->getPathName())] = &fromNode;

    SofaComponentGraphModel* model = dynamic_cast<SofaComponentGraphModel*>(fromNode.nodeDataModel());
    model->setCaption(name);

    auto& fromNgo = fromNode.nodeGraphicsObject();
    fromNgo.setPos(x,y);

    return &fromNode;
}

QtNodes::Node* GraphView::addSimulationObject(sofa::core::objectmodel::BaseObject* bObject, double x, double y)
{
    const std::string& name = bObject->getClassName() + " - " + bObject->getName();
    msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "addSimulationObject: " << name;

    QtNodes::Node& fromNode = m_graphScene->createNode(
                std::make_unique<SofaComponentGraphModel>(bObject, debugNodeGraph));
    fromNode.setObjectName(QString::fromStdString(bObject->getName()));

    m_nodeToQUid[QString::fromStdString(bObject->getPathName())] = &fromNode;

    SofaComponentGraphModel* model = dynamic_cast<SofaComponentGraphModel*>(fromNode.nodeDataModel());
    model->setCaption(name);

    auto& fromNgo = fromNode.nodeGraphicsObject();
    fromNgo.setPos(x,y);

    return &fromNode;
}



void GraphView::connectNodeData()
{
    std::vector <QtNodes::Node*> nodes = m_graphScene->allNodes();

    for (auto node : nodes)
    {
        // get connections
        SofaComponentGraphModel* childNode = dynamic_cast<SofaComponentGraphModel*>(node->nodeDataModel());

        if (childNode->getNbrConnections() == 0)
            continue;

        const std::map <QString, std::pair < QString, QString> >& connections = childNode->getDataConnections();

        for (auto connection : connections)
        {
            bool parentFound = false;
            for (auto pNode : nodes)
            {
                QString pObjName = pNode->objectName();
                if (pObjName.compare(connection.second.first) == 0)
                {
                    parentFound = true;
                    SofaComponentGraphModel* parentNode = dynamic_cast<SofaComponentGraphModel*>(pNode->nodeDataModel());
                    QtNodes::PortIndex parentId = parentNode->getDataInputId(connection.second.second);
                    QtNodes::PortIndex childId = childNode->getDataInputId(connection.first);

                    m_graphScene->createConnection(*node, childId, *pNode, parentId);

                    break;
                }
            }

            if (!parentFound)
            {
                msg_error("SofaWindowDataGraph") << "Object not found while creating connection between " << node->objectName().toStdString() << " and child: " << connection.second.first.toStdString();
                continue;
            }
        }
    }
}

} // namespace sofaqtquick::views
