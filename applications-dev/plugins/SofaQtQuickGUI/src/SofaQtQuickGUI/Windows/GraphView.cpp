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
#include <nodes/FlowScene>
#include <nodes/DataModelRegistry>
#include <nodes/ConnectionStyle>
#include <nodes/Node>

#include <sofa/core/ObjectFactory.h>
#include <sofa/core/ComponentLibrary.h>

#include "GraphView.h"
#include <SofaQtQuickGUI/Models/SofaComponentGraphModel.h>

#include <SofaSimulationGraph/DAGNode.h>
using sofa::simulation::graph::DAGNode;

namespace sofaqtquick::views
{
using namespace sofa::helper;


using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::ConnectionStyle;

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


static
void
setConnecStyle()
{
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
    , debugNodeGraph(false)
{
    setConnecStyle();
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
    debugNodeGraph=true;

    if(m_rootNode==nullptr)
        return;

    // start from parsing root node
    parseSimulationNode(m_rootNode);

    // then connect all data
    connectNodeData();
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

void GraphView::setSelectedComponent(SofaBase* c)
{
    m_selectedComponent = c->rawBase();
    emit selectedComponentChanged();
}

SofaNode* GraphView::getRootNode()
{
    return new SofaNode(DAGNode::SPtr(dynamic_cast<DAGNode*>(m_rootNode->toBaseNode())));
}

void GraphView::setRootNode(SofaNode* node)
{
    std::cout << "SETTING SOFA NODE " << node->getPathName().toStdString() << std::endl;
    m_rootNode = node->self();
    msg_error("ICI ZERO");

    resetNodeGraph(m_rootNode);
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
}

void GraphView::resetNodeGraph(sofa::simulation::Node* scene)
{
    m_rootNode = scene;
    clearNodeData();


    // start from parsing root node
    parseSimulationNode(m_rootNode);

    // then connect all data
    connectNodeData();
}


void GraphView::parseSimulationNode(sofa::simulation::Node* node, int posX)
{
    std::cout << "HLLOE WORLD..XXX" << std::endl;
     if(node==nullptr)
        return;

    std::cout << "HLLOE WORLD..YY" << std::endl;

    msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << m_posY << " ### Child Name: " << node->getName();
    // first parse the list BaseObject inside this node
    std::vector<sofa::core::objectmodel::BaseObject*> bObjects = node->getNodeObjects();
    m_posX = posX;
    int maxData = 0;
    std::cout << "HLLOE WORLD..YY: " << bObjects.size() << std::endl;

    for (auto bObject : bObjects)
    {       
       std::cout << "LA " << bObject->getName() << std::endl;

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
    
        size_t nbrData = addSimulationObject(bObject);
        if (nbrData > maxData)
            maxData = nbrData;

        // space between cells
        m_posX += 14 * m_scaleX;
    }

    if (bObjects.size() >= 4) {
        m_posY += (maxData + 10) * m_scaleY;
        m_posX = posX + 30 * m_scaleX;
    }

    // second move to child nodes
    for (auto simuNode : node->getChildren())
    {
        parseSimulationNode(dynamic_cast<sofa::simulation::Node*>(simuNode), m_posX);
    }
}


size_t GraphView::addSimulationObject(sofa::core::objectmodel::BaseObject* bObject)
{
    std::cout << "ADD OBJECT" << bObject << std::endl;

    const std::string& name = bObject->getClassName() + " - " + bObject->getName();
    msg_info_when(debugNodeGraph, "SofaWindowDataGraph") << "addSimulationObject: " << name;
    
    QtNodes::Node& fromNode = m_graphScene->createNode(std::make_unique<SofaComponentGraphModel>(bObject, debugNodeGraph));
    fromNode.setObjectName(QString::fromStdString(bObject->getName()));
    
    SofaComponentGraphModel* model = dynamic_cast<SofaComponentGraphModel*>(fromNode.nodeDataModel());
    model->setCaption(name);

    auto& fromNgo = fromNode.nodeGraphicsObject();    
    fromNgo.setPos(m_posX, m_posY);
    m_posX += name.length() * m_scaleX;

    return model->getNbrData();
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
