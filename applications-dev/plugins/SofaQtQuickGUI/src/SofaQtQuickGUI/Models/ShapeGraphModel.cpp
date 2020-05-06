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

#include <sofa/core/objectmodel/BaseObject.h>
#include "ShapeGraphModel.h"

namespace sofaqtquick::models
{


static const char* ignoredData[] = { "name", "printLog", "tags", "bbox", "listening", "componentState" };


ShapeGraphNodeData::ShapeGraphNodeData()
    : m_bData(nullptr)
{

}

ShapeGraphNodeData::ShapeGraphNodeData(sofa::core::objectmodel::BaseData* bData)
    : m_bData(bData)
{

}

sofa::core::objectmodel::BaseData* ShapeGraphNodeData::getData()
{
    //m_bData->updateIfDirty(); ??
    return m_bData;
}

NodeDataType ShapeGraphNodeData::type() const
{
    return NodeDataType{ "SofaComponentNodeData",
        "My Sofa Node Data" };
}


ShapeGraphModel::ShapeGraphModel(std::string name)
    : NodeDataModel()
    , debugNodeGraph(true)
    , m_SofaObject(nullptr)
{
    m_uniqName = QString::fromStdString(name);
    m_caption = m_uniqName;
}

ShapeGraphModel::ShapeGraphModel(sofa::core::objectmodel::Base* _sofaObject, bool debugMode)
    : NodeDataModel()
    , debugNodeGraph(debugMode)
    , m_SofaObject(_sofaObject)
{
    if (m_SofaObject == nullptr)
    {
        msg_error("SofaComponentGraphModel") << "Sofa BaseObject is null, Node graph initialisation failed.";
        m_uniqName = "ErrorNode";
    }
    else
    {
        parseSofaObjectData();
    }
}


void ShapeGraphModel::parseSofaObjectData()
{
    if (m_SofaObject == nullptr)
    {
        msg_error("SofaComponentGraphModel") << "Sofa BaseObject is null, Node graph parseSofaObjectData failed.";
        return;
    }

    // first add this object name as first Data (to be used for the links representation)
    QString SObjectName = QString::fromStdString(m_SofaObject->getName());
    m_data.push_back(std::pair<QString, QString>(SObjectName, "name"));

    // parse links
    const sofa::core::objectmodel::Base::VecLink& links = m_SofaObject->getLinks();
    for (auto link : links)
    {
        const std::string& name = link->getName();
        // ignore unnamed link
        if (link->getName().empty())
            continue;

        // ignore link to context
        if (link->getName() == "context")
            continue;

        if (!link->storePath() && 0 == link->getSize())
            continue;

        const std::string valuetype = link->getValueTypeString();

        msg_info_when(debugNodeGraph, "SofaComponentGraphModel") << "## link: " << name << " | link->getSize(): " << link->getSize() << " | valuetype: " << valuetype << " | path: " << link->storePath();

        std::string linkPath = link->getLinkedPath();
        linkPath.erase(0, 1); // remove @
        std::size_t found = linkPath.find_last_of("/");
        if (found != std::string::npos) // remove path
            linkPath.erase(0, found);

        msg_info_when(debugNodeGraph, "SofaComponentGraphModel") << "  # baselink: " << linkPath;
        QString parentObject = QString::fromStdString(linkPath);
        m_dataConnections[SObjectName] = std::pair<QString, QString>(parentObject, parentObject);
    }

    // parse all Data
    sofa::helper::vector<sofa::core::objectmodel::BaseData*> allData = m_SofaObject->getDataFields();
    for (auto data : allData)
    {
        QString name = QString::fromStdString(data->getName());
        QString group = QString::fromStdString(data->getGroup());

        bool toBeIgnored = false;
        for (auto idata : ignoredData)
        {
            if (name.compare(idata) == 0)
            {
                toBeIgnored = true;
                break;
            }
        }

        if (group == "Visualization")
            toBeIgnored = true;


        if (toBeIgnored)
            continue;

        sofa::core::objectmodel::BaseData* pData = data->getParent();
        if (pData)
        {
            QString parentObject = QString::fromStdString(pData->getOwner()->getName());
            QString parentData = QString::fromStdString(pData->getName());
            msg_info_when(debugNodeGraph, "SofaComponentGraphModel") << "- Parent: " << pData->getName() << " owwner: " << pData->getOwner()->getName();
            m_dataConnections[name] = std::pair<QString, QString>(parentObject, parentData);
        }

        if (!group.isEmpty())
        {
            msg_info_when(debugNodeGraph, "SofaComponentGraphModel") << name.toStdString() << " -> " << data->getGroup();
        }
        m_data.push_back(std::pair<QString, QString>(name, QString::fromStdString(data->getValueTypeString())));
        m_Nodedata.push_back(std::make_shared<ShapeGraphNodeData>(data));
    }
}


QtNodes::PortIndex ShapeGraphModel::getDataInputId(const QString& dataName)
{
    int cpt = 0;
    for (auto data : m_data)
    {
        if (data.first.compare(dataName) == 0)
            return cpt;

        cpt++;
    }

    return QtNodes::INVALID;
}


unsigned int ShapeGraphModel::nPorts(PortType portType) const
{
    return m_data.size();
}

NodeDataType ShapeGraphModel::dataType(PortType portType, PortIndex portIndex) const
{
    if (portIndex >= 0 && portIndex < m_data.size())
    {
        NodeDataType NType;
        NType.id = m_data[portIndex].second;
        NType.name = m_data[portIndex].first;
        return NType;
    }

    return ShapeGraphNodeData().type();
}

std::shared_ptr<NodeData> ShapeGraphModel::outData(PortIndex port)
{
    // because the first port is the name of the component not stored in m_Nodedata:
    port--;

    if (port>0 && port < m_Nodedata.size())
        return m_Nodedata[port];
    else {
        msg_warning(m_caption.toStdString()) << "Method SofaComponentGraphModel::outData port: " << port << " out of bounds: " << m_Nodedata.size();
        return nullptr;
    }
}

void ShapeGraphModel::setInData(std::shared_ptr<NodeData> data, int port)
{
    auto parentNodeData = std::dynamic_pointer_cast<ShapeGraphNodeData>(data);
    if (parentNodeData == nullptr)
    {
        msg_warning(m_caption.toStdString()) << "Method SofaComponentGraphModel::setInData SofaComponentNodeData cast failed.";
        return;
    }

    // because the first port is the name of the component not stored in m_Nodedata:
    port--;

    if (port < 0 || port >= m_Nodedata.size())
    {
        msg_warning(m_caption.toStdString()) << "Method SofaComponentGraphModel::setInData port: "<< port << " out of bounds: " << m_Nodedata.size();
        return;
    }

    // here you will implement the Data setParent in SOFA!
    std::shared_ptr<ShapeGraphNodeData> childNodeData = this->m_Nodedata[port];
    sofa::core::objectmodel::BaseData* childData = childNodeData->getData();
    sofa::core::objectmodel::BaseData* parentData = parentNodeData->getData();

    msg_info_when(debugNodeGraph, m_caption.toStdString()) << "Here connect: {" << parentData->getOwner()->getName() << ", " << parentData->getName() << "} -> {"
        << childData->getOwner()->getName() << ", " << childData->getName() << "}";

}

} /// namespace sofaqtquick::models
