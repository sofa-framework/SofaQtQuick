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

#include "SofaDataListModel.h"

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

SofaDataListModel::SofaDataListModel(QObject* parent) : QAbstractListModel(parent),
    myItems(),
    myUpdatedCount(0),
    mySofaComponent(0)
{
    connect(this, &SofaDataListModel::sofaComponentChanged, &SofaDataListModel::update);
}

SofaDataListModel::~SofaDataListModel()
{

}

void SofaDataListModel::update()
{
    myItems.clear();

    if(mySofaComponent)
    {
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

    /*int changeNum = myItems.size() - myUpdatedCount;
    if(changeNum > 0)
    {
        beginInsertRows(QModelIndex(), myUpdatedCount, myItems.size() - 1);
        endInsertRows();
    }
    else if(changeNum < 0)
    {
        beginRemoveRows(QModelIndex(), myItems.size(), myUpdatedCount - 1);
        endRemoveRows();
    }

    dataChanged(createIndex(0, 0), createIndex(myItems.size() - 1, 0));*/

    // TODO: why the old system does not work ?

    beginRemoveRows(QModelIndex(), 0, myUpdatedCount - 1);
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, myItems.size() - 1);
    endInsertRows();

    myUpdatedCount = myItems.size();
}

SofaDataListModel::Item SofaDataListModel::buildItem(BaseData* data) const
{
    SofaDataListModel::Item item;

    item.name = QString::fromStdString(data->getName());
    item.group = data->getGroup();
    item.type = SofaDataType;
    item.data = QVariant::fromValue((void*) data);

    return item;
}

SofaDataListModel::Item SofaDataListModel::buildItem(BaseLink* link) const
{
    SofaDataListModel::Item item;

    item.name = QString::fromStdString(link->getName());
    item.group = "Links";
    item.type = SofaLinkType;
    item.data = QVariant::fromValue((void*) link);

    return item;
}

SofaDataListModel::Item SofaDataListModel::buildItem(const QString& name, const QString& group, const QString& value) const
{
    SofaDataListModel::Item item;

    item.name = name;
    item.group = group;
    item.type = "Log" == group ? LogType : InfoType;
    item.data = QVariant::fromValue(value);

    return item;
}

void SofaDataListModel::setSofaComponent(SofaComponent* newSofaComponent)
{
    if(newSofaComponent == mySofaComponent)
        return;

    mySofaComponent = newSofaComponent;

    sofaComponentChanged(newSofaComponent);
}

int	SofaDataListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return myItems.size();
}

QVariant SofaDataListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
    {
        msg_error("SofaQtQuickGUI") << "SofaDataListModel::data  Invalid index";
        return QVariant("");
    }

    if(index.row() >= myItems.size())
        return QVariant("");

    if(!mySofaComponent->base())
    {
        //the base is not valid anymore, neither is its data
        mySofaComponent = 0;
        sofaComponentChanged(0);

        return QVariant("");
    }

    const Item& item = myItems[index.row()];

    switch(role)
    {
    case NameRole:
        return QVariant::fromValue(item.name);
    case GroupRole:
    {
        QString group = item.group;
        if(group.isEmpty())
            group = "Base";

        return QVariant::fromValue(group);
    }
    case TypeRole:
        return item.type;
    case ValueRole:
    {
        if(SofaDataType == item.type)
        {
            BaseData* data = (sofa::core::objectmodel::BaseData*) item.data.value<void*>();

            return QVariant::fromValue(SofaScene::dataValue(data));
        }
        else if(SofaLinkType == item.type)
        {
            BaseLink* link = (sofa::core::objectmodel::BaseLink*) item.data.value<void*>();

            return QVariant::fromValue(SofaScene::linkValue(link));
        }
		else if(LogType == item.type)
		{
			return QVariant::fromValue(item.data.toString());
		}
        else if(InfoType == item.type)
        {
            return QVariant::fromValue(item.data.toString());
        }
    }
    default:
        msg_error("SofaQtQuickGUI") << "SofaDataListModel::data  Role unknown:" << role;
        break;
    }

    return QVariant("");
}

QHash<int,QByteArray> SofaDataListModel::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[NameRole]         = "name";
    roles[GroupRole]        = "group";
    roles[TypeRole]         = "type";
    roles[ValueRole]        = "value";

    return roles;
}

SofaData* SofaDataListModel::getDataById(int row) const
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
