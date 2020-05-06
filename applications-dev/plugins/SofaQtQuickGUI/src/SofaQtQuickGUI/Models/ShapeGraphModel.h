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
    - damien.marchal@univ-lille.fr
********************************************************************/
#pragma once

#include <QtCore/QObject>

#define NODE_EDITOR_SHARED
#include <nodes/NodeData>
#include <nodes/NodeDataModel>

#include <map>

namespace sofa::core::objectmodel
{
class BaseObject;
class BaseData;
class Base;
}


namespace sofaqtquick::models
{

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::PortType;
using QtNodes::PortIndex;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class ShapeGraphNodeData : public NodeData
{
public:
    ShapeGraphNodeData();
    ShapeGraphNodeData(sofa::core::objectmodel::BaseData* bData);

    NodeDataType type() const override;

    sofa::core::objectmodel::BaseData* getData();

protected:
    sofa::core::objectmodel::BaseData* m_bData;
};


//------------------------------------------------------------------------------
/**
* This Class is a NodeDataModel specialisation to represent a Sofa component on the QtNodes graph.
* It will take a SOFA BaseObject as target and parse all Data, storing Data, Links and connections with parents components.
*/
class ShapeGraphModel : public NodeDataModel
{
    Q_OBJECT

public:
    /// Default empty Object constructor with 0 Data
    ShapeGraphModel(std::string name = "EmptyNode");

    /// constructor with a Sofa BaseObject as target
    ShapeGraphModel(sofa::core::objectmodel::Base* _sofaObject, bool debugMode = false);

    virtual ~ShapeGraphModel() override {}

    /// Interface for caption.
    QString caption() const override {return m_caption;}
    void setCaption(std::string str) {m_caption = QString::fromStdString(str);}

    /// Interface for name.
    QString name() const override { return m_uniqName; }

    /// Return the number of Data.
    size_t getNbrData() {return m_data.size();}

    /// Return the number of connection with other Node components
    size_t getNbrConnections() { return m_dataConnections.size(); }

    /// return the list of connections @sa m_dataConnections
    const std::map <QString, std::pair < QString, QString> >& getDataConnections() { return m_dataConnections; }

    /// Return the PortIndex of a Data given its Name.
    QtNodes::PortIndex getDataInputId(const QString& dataName);

    ///Interface for QtNodes
    ///{
    /// Override method to return the number of ports
    unsigned int nPorts(PortType portType) const override;

    /// Override method to give the type of Data per Port
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    /// Override method to return the NodeData given a port
    std::shared_ptr<NodeData> outData(PortIndex port) override;

    /// Override method to set input Data
    void setInData(std::shared_ptr<NodeData> data, int port) override;

    /// Override method for more advance node gui. Not yet used.
    QWidget * embeddedWidget() override { return nullptr; }
    ///}

protected:
    /// Internal method to parse all Data of a Sofa component and create the corresponding ports
    void parseSofaObjectData();

protected:
    QString m_caption; ///< caption to be display on the Graph
    QString m_uniqName; ///< unique name to refer to this node

    bool debugNodeGraph; ///< parameter to activate graph logs. False by default.

    /// Vector of Data/port hold by this component/Node. vector of pair{DataName, DataType}
    std::vector < std::pair < QString, QString> > m_data;

    /// vector of SofaComponentNodeData class holding pointer to the Data. To replace @sa m_data when api is validated.
    std::vector < std::shared_ptr<ShapeGraphNodeData> > m_Nodedata;

    /// Map to store all connection between this node and other. map.key = this data name, map.value = pair{ComponentName, DataName}
    std::map <QString, std::pair < QString, QString> > m_dataConnections;

    /// Pointer to the sofa object.
    sofa::core::objectmodel::Base* m_SofaObject;
};

} /// sofaqtquick::models
