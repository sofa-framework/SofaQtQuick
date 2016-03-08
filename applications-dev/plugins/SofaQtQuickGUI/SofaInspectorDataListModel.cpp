/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SofaInspectorDataListModel.h"

#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/logging/Messaging.h>

#include <QStack>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::simulation;

SofaInspectorDataListModel::SofaInspectorDataListModel(QObject* parent) : QAbstractListModel(parent),
    myItems(),
    myUpdatedCount(0),
    mySofaComponent(0)
{
    connect(this, &SofaInspectorDataListModel::sofaComponentChanged, &SofaInspectorDataListModel::update);
}

SofaInspectorDataListModel::~SofaInspectorDataListModel()
{

}

void SofaInspectorDataListModel::update()
{
    myItems.clear();

    if(mySofaComponent)
    {
        msg_info("SofaInspectorDataListModel") <<
        "item reconstructed for: <b> " << mySofaComponent->name().toStdString() << "</b>";

        const Base* base = mySofaComponent->base();
        if(base)
        {
            // SofaData
            Base::VecData dataFields = base->getDataFields();
            for(unsigned int i = 0; i < dataFields.size(); ++i)
            {
                BaseData* data = dataFields[i];
                if(data->isDisplayed())
                    myItems.append(buildItem(dataFields[i]));
            }

            // Links
            Base::VecLink linkFields = base->getLinks();
            for(unsigned int i = 0; i < linkFields.size(); ++i)
                myItems.append(buildItem(linkFields[i]));

            // Logs & Warnings
            QString logGroup = "Log";
            myItems.append(buildItem("output", logGroup, QString::fromStdString(base->getOutputs())));
            myItems.append(buildItem("warning", logGroup, QString::fromStdString(base->getWarnings())));

            // Info
            QString infoGroup = "Info";
            myItems.append(buildItem("name", infoGroup, QString::fromStdString(base->getName())));
            myItems.append(buildItem("class", infoGroup,QString::fromStdString(base->getClassName())));

            std::string namespaceName = core::objectmodel::BaseClass::decodeNamespaceName(typeid(*base));
            if(!namespaceName.empty())
                myItems.append(buildItem("namespace", infoGroup,QString::fromStdString(namespaceName)));

            std::string templateName = base->getTemplateName();
            if(!templateName.empty())
                myItems.append(buildItem("template", infoGroup,QString::fromStdString(templateName)));

            core::ObjectFactory::ClassEntry entry = core::ObjectFactory::getInstance()->getEntry(base->getClassName());
            if(!entry.creatorMap.empty())
            {
                if(!entry.description.empty() && std::string("TODO") != entry.description)
                    myItems.append(buildItem("description", infoGroup,QString::fromStdString(entry.description)));

                core::ObjectFactory::CreatorMap::iterator it = entry.creatorMap.find(base->getTemplateName());
                if(entry.creatorMap.end() != it && *it->second->getTarget())
                    myItems.append(buildItem("provided by", infoGroup,QString::fromStdString(it->second->getTarget())));
            }

            // Sort attributes by group
            qStableSort(myItems.begin(), myItems.end(), [](const Item& a, const Item& b) {return QString::compare(a.group, b.group) < 0;});
        }
        else
        {
            setSofaComponent(0);
        }
    }

    beginRemoveRows(QModelIndex(), 0, myUpdatedCount - 1);
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, myItems.size() - 1);
    endInsertRows();

    myUpdatedCount = myItems.size();
}

SofaInspectorDataListModel::Item SofaInspectorDataListModel::buildItem(BaseData* data) const
{
    SofaInspectorDataListModel::Item item;

    item.name = QString::fromStdString(data->getName());
    item.group = data->getGroup();
    item.type = SofaDataType;
    item.data = QVariant::fromValue((void*) data);

    return item;
}

SofaInspectorDataListModel::Item SofaInspectorDataListModel::buildItem(BaseLink* link) const
{
    SofaInspectorDataListModel::Item item;

    item.name = QString::fromStdString(link->getName());
    item.group = "Links";
    item.type = SofaLinkType;
    item.data = QVariant::fromValue((void*) link);

    return item;
}

SofaInspectorDataListModel::Item SofaInspectorDataListModel::buildItem(const QString& name, const QString& group, const QString& value) const
{
    SofaInspectorDataListModel::Item item;

    item.name = name;
    item.group = group;
    item.type = "Log" == group ? LogType : InfoType;
    item.data = QVariant::fromValue(value);

    return item;
}

void SofaInspectorDataListModel::setSofaComponent(SofaComponent* newSofaComponent)
{
    if(newSofaComponent == mySofaComponent)
        return;

    mySofaComponent = newSofaComponent;

    sofaComponentChanged(newSofaComponent);
}

int	SofaInspectorDataListModel::rowCount(const QModelIndex& parent) const
{
    dmsg_error("SofaQtQuickGUI") << "row count" << parent.row() << ", " << parent.column() ;
    if( parent.row() == 0 & parent.column() == 0 )
        return 5;
    return 0;
}

QVariant SofaInspectorDataListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
    {
        msg_error("SofaQtQuickGUI") << "SofaInspectorDataListModel::data  Invalid index";
        return QVariant("");
    }

    return QVariant("Damien") ;
}

QHash<int,QByteArray> SofaInspectorDataListModel::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[NameRole]         = "name";
    roles[GroupRole]        = "group";
    roles[TypeRole]         = "type";
    roles[ValueRole]        = "value";

    return roles;
}

SofaData* SofaInspectorDataListModel::getDataById(int row) const
{
    if(row < 0 || row >= myItems.size())
        return 0;

    const QVariant& data = myItems.at(row).data;
    if(QVariant::String != data.type())
        return new SofaData(mySofaComponent, (sofa::core::objectmodel::BaseData*) data.value<void*>());

    return nullptr;
}

}

}
