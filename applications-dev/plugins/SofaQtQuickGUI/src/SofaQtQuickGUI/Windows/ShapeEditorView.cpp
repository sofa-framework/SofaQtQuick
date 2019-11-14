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

#include <SofaQtQuickGUI/Windows/ShapeEditorView.h>
using sofaqtquick::views::ShapeGraphView;

#include <SofaQtQuickGUI/Models/ShapeGraphModel.h>
using sofaqtquick::models::ShapeGraphModel;

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

void ShapeGraphMutationListener::onBeginAddObject(Node* parent, BaseObject* obj)
{
    SOFA_UNUSED(obj);
}

void ShapeGraphMutationListener::onEndAddObject(Node* parent, BaseObject* obj)
{
    emit addObject(parent, obj);
}


static std::shared_ptr<DataModelRegistry> registerDataModels()
{
    auto ret = std::make_shared<DataModelRegistry>();

    std::vector<sofa::core::ClassEntry::SPtr> results;
    sofa::core::ObjectFactory::getInstance()->getAllEntries(results);

    ret->registerModel<ShapeGraphModel>();
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

ShapeGraphView::ShapeGraphView(QWidget *parent)
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

    connect(&m_mutationListener, &ShapeGraphMutationListener::addObject, this, &ShapeGraphView::onAddObject);

    if(m_rootNode==nullptr)
        return;

    m_rootNode->addListener(&m_mutationListener);

    // start from parsing root node
    parseSimulationNode(m_rootNode, 0, 0);

    // then connect all data
    connectNodeData();

}

void ShapeGraphView::onAddObject(Node* parent, BaseObject* object)
{
    QtNodes::Node& addedNode = m_graphScene->createNode(
                std::make_unique<ShapeGraphModel>(object, debugNodeGraph));

    m_nodeToQUid[QString::fromStdString(object->getPathName())] = &addedNode;
    addedNode.setObjectName(QString::fromStdString(object->getName()));

    ShapeGraphModel* model = dynamic_cast<ShapeGraphModel*>(addedNode.nodeDataModel());
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

ShapeGraphView::~ShapeGraphView()
{
    std::cout << "SofaWindowDataGraph::~SofaWindowDataGraph()" << std::endl;
    clearNodeData();
    // todo check if m_graphView need to be deleted. Normally no as child of QtWidget RealGui.
    //delete m_graphView;
}

SofaBase* ShapeGraphView::getSelectedComponent()
{
    return new SofaBase(m_selectedComponent);
}


void ShapeGraphView::moveTo(Base* base)
{
}

void ShapeGraphView::setSelectedComponent(SofaBase* base)
{
    m_selectedComponent = base->rawBase();
    emit selectedComponentChanged();
}

SofaNode* ShapeGraphView::getRootNode()
{
    return new SofaNode(DAGNode::SPtr(dynamic_cast<DAGNode*>(m_rootNode->toBaseNode())));
}

void ShapeGraphView::setRootNode(SofaNode* node)
{
    resetNodeGraph(node->self());
    emit rootNodeChanged();
}

void ShapeGraphView::clearNodeData()
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

void ShapeGraphView::resetNodeGraph(sofa::simulation::Node* scene)
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

QVector2D ShapeGraphView::getViewPosition()
{
    return m_viewPosition;
}

void ShapeGraphView::setViewPosition(QVector2D viewPosition)
{
    QVector2D dx = viewPosition-m_viewPosition;
    m_viewPosition = viewPosition;

    m_graphView->translate(dx.x(), dx.y());
    std::cout << "TP" << m_graphView->pos().x() << " => " << m_graphView->pos().y() << std::endl;


    emit viewPositionChanged();
}

void ShapeGraphView::showConnectedComponents(SofaBase* base)
{
    moveTo(base->rawBase());
}

void ShapeGraphView::findConnectedComponents(Base* base)
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
                std::make_unique<ShapeGraphModel>(object, debugNodeGraph));

    m_nodeToQUid[QString::fromStdString(object->getPathName())] = &fromNode;
    fromNode.setObjectName(QString::fromStdString(base->getName()));

    ShapeGraphModel* model = dynamic_cast<ShapeGraphModel*>(fromNode.nodeDataModel());
    model->setCaption(object->getName());

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

QRectF ShapeGraphView::parseSimulationNode(sofa::simulation::Node* node, double posX, double posY)
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


QtNodes::Node* ShapeGraphView::addSimulationNode(sofa::core::objectmodel::BaseNode* node, double x, double y)
{
    const std::string& name = node->getClassName() + " - " + node->getName();
    msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "addSimulationObject: " << name;

    QtNodes::Node& fromNode = m_graphScene->createNode(
                std::make_unique<ShapeGraphModel>(node, debugNodeGraph));
    fromNode.setObjectName(QString::fromStdString(node->getName()));

    m_nodeToQUid[QString::fromStdString(node->getPathName())] = &fromNode;

    ShapeGraphModel* model = dynamic_cast<ShapeGraphModel*>(fromNode.nodeDataModel());
    model->setCaption(name);

    auto& fromNgo = fromNode.nodeGraphicsObject();
    fromNgo.setPos(x,y);

    return &fromNode;
}

QtNodes::Node* ShapeGraphView::addSimulationObject(sofa::core::objectmodel::BaseObject* bObject, double x, double y)
{
    const std::string& name = bObject->getClassName() + " - " + bObject->getName();
    msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "addSimulationObject: " << name;

    QtNodes::Node& fromNode = m_graphScene->createNode(
                std::make_unique<ShapeGraphModel>(bObject, debugNodeGraph));
    fromNode.setObjectName(QString::fromStdString(bObject->getName()));

    m_nodeToQUid[QString::fromStdString(bObject->getPathName())] = &fromNode;

    ShapeGraphModel* model = dynamic_cast<ShapeGraphModel*>(fromNode.nodeDataModel());
    model->setCaption(name);

    auto& fromNgo = fromNode.nodeGraphicsObject();
    fromNgo.setPos(x,y);

    return &fromNode;
}



void ShapeGraphView::connectNodeData()
{
    std::vector <QtNodes::Node*> nodes = m_graphScene->allNodes();

    for (auto node : nodes)
    {
        // get connections
        ShapeGraphModel* childNode = dynamic_cast<ShapeGraphModel*>(node->nodeDataModel());

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
                    ShapeGraphModel* parentNode = dynamic_cast<ShapeGraphModel*>(pNode->nodeDataModel());
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
