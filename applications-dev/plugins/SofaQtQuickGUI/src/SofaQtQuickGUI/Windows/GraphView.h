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
    - erik.pernod
    - damien.marchal@univ-lille.fr
********************************************************************/

#pragma once

#include <QDialog>
#include <QVector2D>
#include <QMap>

#include <sofa/simulation/Node.h>

#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Bindings/SofaNode.h>
namespace QtNodes
{
class FlowScene;
class FlowView;
class Node;
}


#include <QAbstractAnimation>

namespace sofaqtquick::views
{
using sofaqtquick::bindings::SofaBase;
using sofaqtquick::bindings::SofaNode;
using sofa::core::objectmodel::Base;
using sofa::core::objectmodel::BaseObject;
using sofa::core::objectmodel::BaseNode;
using sofa::simulation::MutationListener ;
using sofa::simulation::Node;

class MyMutationListener : public QObject, public MutationListener
{
    Q_OBJECT
public:
    void onBeginAddObject(Node* parent, BaseObject* obj);
    void onEndAddObject(Node* parent, BaseObject* obj);

signals:
    void addObject(Node* parent, BaseObject* obj);
};


class MoveTooAnimation : public QAbstractAnimation
{
    Q_OBJECT

public:
    QPointF m_currentPosition ;
    QPointF m_sourcePosition ;
    QPointF m_destPosition ;

public:
    void set(const QPointF& start, const QPointF& dest)
    {
        m_sourcePosition = start;
        m_currentPosition = start;
        m_destPosition = dest;
    }

    std::function<void(double d, MoveTooAnimation* )> m_cb;
    MoveTooAnimation(std::function<void(double, MoveTooAnimation* a)> cb)
    {
        m_cb = cb;
    }

    int duration() const override { return 1000.0; }
    void updateCurrentTime(int currentTime) override
    {
        m_cb(((double)currentTime)/duration(), this);
    }
};

/**
 * This Class provide an interface with the library QtNodes to display Data Graph connection inside a QDialog.
 * It will take a SOFA simulation scene and create Graph nodes for each Sofa component and display connections between Data.
 */
class GraphView : public QDialog
{
    Q_OBJECT
public:
    Q_PROPERTY(SofaBase* selectedComponent READ getSelectedComponent WRITE setSelectedComponent NOTIFY selectedComponentChanged)
    Q_PROPERTY(SofaNode* rootNode READ getRootNode WRITE setRootNode NOTIFY rootNodeChanged)
    Q_PROPERTY(QVector2D viewPosition READ getViewPosition WRITE setViewPosition NOTIFY viewPositionChanged)

    Q_INVOKABLE void showConnectedComponents(SofaBase*);
    SofaBase* getSelectedComponent();
    void setSelectedComponent(SofaBase*);

    SofaNode* getRootNode();
    void setRootNode(SofaNode*);

    QVector2D getViewPosition();
    void setViewPosition(QVector2D);

    /// Default constructor of the Widget, given a QWidget as parent and a pointer to the current simulation scene.
    GraphView(QWidget *parent);

    ~GraphView();

    /// Method to be called when graph need to be recomputed (like reloading scene). Take a pointer to the root node of the scene.
    void resetNodeGraph(sofa::simulation::Node* scene);

public slots:
   void onAddObject(Node* parent, BaseObject* object);

signals:
    void selectedComponentChanged();
    void rootNodeChanged();
    void viewPositionChanged();

protected:
    /// Internal method to parse all Sofa component inside a Sofa simulation Node. Will call @sa addSimulationObject for each compoenent then will iterate on children nodes.
    QRectF parseSimulationNode(sofa::simulation::Node* node, double posX, double posY);

    /// search for the connected component to display the corresponding graph.
    void findConnectedComponents(Base *base);

    /// Internal method to create a Node for this sofa BaseObject.
    QtNodes::Node* addSimulationObject(sofa::core::objectmodel::BaseObject* bObject, double x, double y);
    QtNodes::Node* addSimulationNode(sofa::core::objectmodel::BaseNode* node, double x, double y);

    /// Internal method to create all connection between component on the graph.
    void connectNodeData();

    /// Internal method to clear the graph structures
    void clearNodeData();

    void moveTo(Base*);
protected:
    /// Pointer to the graphScene used to store nodes.
    QtNodes::FlowScene* m_graphScene;
    /// Pointer to the view of the graph.
    QtNodes::FlowView* m_graphView;

    /// Point to the root node of the current simulation.
    sofa::simulation::Node* m_rootNode {nullptr};
    sofa::core::objectmodel::Base* m_selectedComponent {nullptr};

    /// List of component name not to be display in the graph.
    std::vector<std::string> m_exceptions;

    SReal m_scaleX; ///< Scale paramater to apply between nodes for display in abscissa.
    SReal m_scaleY; ///< Scale paramater to apply between nodes for display in ordinate.

    int m_posX; ///< Increment position counter on abscissa for Node display.
    int m_posY; ///< Increment position counter on ordinate for Node display.

    QVector2D m_viewPosition;

    QMap<QString, QtNodes::Node*> m_nodeToQUid;

    MyMutationListener m_mutationListener;
    bool debugNodeGraph {false}; ///< parameter to activate graph logs. False by default.
};

} // namespace sofaqtquick::views

