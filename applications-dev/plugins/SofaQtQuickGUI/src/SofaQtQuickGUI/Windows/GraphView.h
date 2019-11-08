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

#pragma once

#include <QDialog>

#include <sofa/simulation/Node.h>

#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Bindings/SofaNode.h>

namespace QtNodes
{
    class FlowScene;
    class FlowView;
}


namespace sofaqtquick::views
{
using sofaqtquick::bindings::SofaBase;
using sofaqtquick::bindings::SofaNode;

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

    SofaBase* getSelectedComponent();
    void setSelectedComponent(SofaBase*);

    SofaNode* getRootNode();
    void setRootNode(SofaNode*);

    /// Default constructor of the Widget, given a QWidget as parent and a pointer to the current simulation scene.
    GraphView(QWidget *parent);

    ~GraphView();

    /// Method to be called when graph need to be recomputed (like reloading scene). Take a pointer to the root node of the scene.
    void resetNodeGraph(sofa::simulation::Node* scene);

signals:
    void selectedComponentChanged();
    void rootNodeChanged();

protected:
    /// Internal method to parse all Sofa component inside a Sofa simulation Node. Will call @sa addSimulationObject for each compoenent then will iterate on children nodes.
    void parseSimulationNode(sofa::simulation::Node* node, int posX = 0);

    /// Internal method to create a Node for this sofa BaseObject.
    size_t addSimulationObject(sofa::core::objectmodel::BaseObject* bObject);

    /// Internal method to create all connection between component on the graph.
    void connectNodeData();

    /// Internal method to clear the graph structures
    void clearNodeData();       

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

    bool debugNodeGraph; ///< parameter to activate graph logs. False by default.
};

} // namespace sofaqtquick::views

