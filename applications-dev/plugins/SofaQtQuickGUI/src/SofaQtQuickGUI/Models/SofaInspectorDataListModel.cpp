/*
Contributors: damien.marchal@univ-lille1.fr
Copyright 2016 CNRS,

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
#include <QSettings>

#include "SofaInspectorDataListModel.h"

#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/logging/Messaging.h>
using sofa::helper::logging::Message ;

namespace sofaqtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::simulation;

SofaInspectorDataListModel::SofaInspectorDataListModel(QObject* parent) : QAbstractItemModel(parent),
    m_groups(),
    m_currentSofaComponent(0)
{
}

SofaInspectorDataListModel::~SofaInspectorDataListModel()
{}

// Search for a group with a O(n.ln) complexity.
ItemGroup* SofaInspectorDataListModel::findOrCreateGroup(const QString& name)
{
    ItemGroup* found=nullptr;
    auto it = m_nameindex.find(name) ;
    if( it == m_nameindex.end() ){
        found = new ItemGroup(name);
        m_groups.append(found);
        m_nameindex[name]=m_groups.size()-1;
    }else{
        found = m_groups[it.value()];
    }
    return found;
}

const QString& getGroupName(const QString& a, const QString& b)
{
    if(a.size()!=0)
        return a ;
    return b ;
}

void SofaInspectorDataListModel::update()
{
    QSettings settings;

    // TODO(dmarchal): delete memory !
    for(auto& group : m_groups)
        delete group;
    m_groups.clear();
    m_nameindex.clear();

    if(m_currentSofaComponent && m_currentSofaComponent->rawBase())
    {
        const Base* base = m_currentSofaComponent->rawBase();
        if(base)
        {
            QStringList listinfos ;
            // SofaData
            QString baseString("Base");
            for(BaseData* data : base->getDataFields())
            {
                std::string gname = data->getGroup() ;
                if(gname.empty()){
                    gname = data->getOwnerClass();
                }

                if( QString("Context") == QString(gname.c_str()) )
                {
                    gname="Base";
                }
                else if( QString("BaseObject") == QString(gname.c_str()) )
                {
                    gname="Base";
                }

                if( QString("Infos") != QString(gname.c_str()) )
                {
                    ItemGroup* ig = findOrCreateGroup(getGroupName(QString(gname.c_str()),
                                                                   baseString)) ;
                    ig->m_children.append(new Item(QString::fromStdString(data->getName()),
                                                   QVariant::fromValue((void*)data),
                                                   SofaDataType,
                                                   settings.value("inspector/"+ig->m_name+"/"+QString::fromStdString(data->getName()), true).toBool()
                                                   ));
                }else
                {
                    listinfos.append(QString::fromStdString(data->getName()));
                }

            }

            // SofaLink
            ItemGroup* links = findOrCreateGroup("Links") ;
            for(BaseLink* link : base->getLinks()){
                links->m_children.append(new Item(QString::fromStdString(link->getName()),
                                                  QVariant::fromValue((void*)link),
                                                  SofaLinkType, true));
            }

            // Logs & Warnings
            /**
            QString logName("Messages");
            ItemGroup* logs = findOrCreateGroup(logName) ;
            logs->m_children.append(new Item("info",
                                             QString::fromStdString(base->getLoggedMessagesAsString({Message::Advice,
                                                                                                     Message::Info})),
                                             LogType, true));

            logs->m_children.append(new Item("warning",
                                             QString::fromStdString(base->getLoggedMessagesAsString({Message::Deprecated,
                                                                                                     Message::Warning,
                                                                                                     Message::Error,
                                                                                                     Message::Fatal})),
                                             LogType, true));
            */

            // Infos
            QString infoGroup = "Infos";
            ItemGroup* infos = findOrCreateGroup(infoGroup);
            infos->m_children.append(new Item("name",
                                              QString::fromStdString(base->getName()), InfoType, true));
            infos->m_children.append(new Item("class",
                                              QString::fromStdString(base->getClassName()), InfoType, true));

            for(auto& info : listinfos)
            {
                BaseData* data = base->findData(info.toStdString()) ;
                infos->m_children.append(new Item(QString::fromStdString(data->getName()),
                                                  QString::fromStdString(data->getValueString()), InfoType, true));
            }

            // Retrieve the group visilibity from the settings.
            for(ItemGroup* group : m_groups){
                group->setVisibility(settings.value("inspector/"+group->m_name, true).toBool() );
                group->m_order = settings.value("inspector/ordering/"+group->m_name, 0).toInt() ;
            }

            // Sort the entries using the qStableSort function and a lambda comparator
            // function. Lambdas are C++x11 feature.
            qStableSort(m_groups.begin(), m_groups.end(),
                        [](const ItemGroup* a, const ItemGroup* b) {return a->m_order<b->m_order;});

            // Currently sort in alphabetical sorting.If you have better suggestion. Please propose.
            for(auto& group : m_groups){
                auto& items = group->m_children;
                qStableSort(items.begin(), items.end(),
                            [](const Item* a, const Item* b) {return a->m_name<b->m_name;});
            }

        }
    }
}

void SofaInspectorDataListModel::setCurrentSofaComponent(SofaBase* newSofaComponent)
{
    if(newSofaComponent == m_currentSofaComponent)
        return;

    if(newSofaComponent && m_currentSofaComponent && newSofaComponent->rawBase() == m_currentSofaComponent->rawBase())
        return;

    //    std::cout << "SofaInspectorDataListMModel::setCurrentSofaComponent begin sofabase: "<< newSofaComponent;
    //    if(newSofaComponent){
    //        std::cout << "component: " << newSofaComponent;
    //        if(newSofaComponent->base())
    //            std::cout << " base: " << newSofaComponent->base()->getName();
    //    }

    beginResetModel();
    m_currentSofaComponent = newSofaComponent;

    update();
    endResetModel();
    //    std::cout << "SofaInspectorDataListMModel::setCurrentSofaComponent end " << std::endl;

}

void SofaInspectorDataListModel::handleDataHasChanged()
{
    // The data has changed... we should try to locate where the mess is and
    // rebuild ?
    beginResetModel();
    endResetModel();
}

QModelIndex SofaInspectorDataListModel::parent ( const QModelIndex & index ) const
{
    return index.parent() ;
}

QModelIndex SofaInspectorDataListModel::index (int row, int column,
                                               const QModelIndex& parent) const
{
    //std::cout << "index" << row << ", " << column << std::endl;

    if (!parent.isValid() ){
        if(row < 0 || row>=m_groups.size())
            return QModelIndex() ;

        QModelIndex m = createIndex(row, column, (void*) m_groups[row]) ;
        return m;
    }

    ItemGroup* ig=dynamic_cast<ItemGroup*>(static_cast<ItemBase*>(parent.internalPointer()));
    if(ig){
        return createIndex(row, column, ig->m_children[row]) ;
    }

    return QModelIndex() ;
}

int	SofaInspectorDataListModel::columnCount(const QModelIndex & parent) const
{
    SOFA_UNUSED(parent) ;
    return 1;
}

int	SofaInspectorDataListModel::rowCount(int index)
{
    // std::cout << "rowCount(index): " << index << std::endl;
    if(index < 0)
        return m_groups.size();
    return m_groups[index]->m_children.size() ;
}

void SofaInspectorDataListModel::setVisibility(int groupid, bool visibility)
{
    if(groupid<0)
        return;

    QSettings settings;
    settings.setValue("inspector/"+m_groups[groupid]->m_name, visibility);
    m_groups[groupid]->setVisibility(visibility);
}

void SofaInspectorDataListModel::setVisibility(int groupid, int itemid, bool visibility)
{
    if(groupid<0 || itemid<0)
        return;
    QSettings settings;
    settings.setValue("inspector/"+m_groups[groupid]->m_name+"/"+m_groups[groupid]->m_children[itemid]->m_name, visibility);
    m_groups[groupid]->m_children[itemid]->setVisibility(visibility);
}

void SofaInspectorDataListModel::setOrdering(int groupid, int idx)
{
    QSettings settings;
    settings.setValue("inspector/ordering/"+m_groups[groupid]->m_name, idx);

    beginResetModel();
    m_groups[groupid]->m_order += idx ;
    qStableSort(m_groups.begin(), m_groups.end(),
                [](const ItemGroup* a, const ItemGroup* b) {return a->m_order<b->m_order;});
    endResetModel();
}

int	SofaInspectorDataListModel::rowCount(const QModelIndex& index) const
{
    //std::cout << "rowCount(modelindex): " << index.row() << " -> " << index.isValid()<< " size: " << m_groups.size() << std::endl;
    if(!index.isValid())
    {
        return m_groups.size() ;
    }

    ItemGroup* ig=dynamic_cast<ItemGroup*>(static_cast<ItemBase*>(index.internalPointer()));
    if(ig){
        //std::cout << "internal pointer" << std::endl;
        return ig->m_children.size();
    }
    return 0;
}

QVariant SofaInspectorDataListModel::data(const QModelIndex& index, int role) const
{
    //std::cout << "data... " << std::endl;

    // Returns the root of the hierarchy
    if(!index.isValid()){
        return QVariant("");
    }

    if(index.internalPointer() == nullptr){
        dmsg_error("SofaInspectorDataListModel") << "invalid operation";
        return QVariant() ;
    }

    ItemBase* ib=static_cast<ItemBase*>(index.internalPointer());
    ItemGroup* ig=dynamic_cast<ItemGroup*>(ib);

    if(ig){
        switch(role){
        case NameRole:
            return QVariant(ig->m_name);
        case TypeRole:
            return QVariant(GroupType);
        case ValueRole:
            return QVariant("");
        default:
            break;
        }
        dmsg_error("SofaInspectorDataListModel") << "undefined role";
        return QVariant() ;
    }
    Item* item = static_cast<Item*>(index.internalPointer());
    if(item){
        switch(role){
        case NameRole:
            return  QVariant::fromValue(item->m_name);
        case TypeRole:
            return QVariant(item->m_type);
        case ReadOnlyRole:
            if(SofaDataType == item->m_type)
            {
                BaseData* data = (BaseData*) item->m_data.value<void*>();
                return QVariant::fromValue( data->isReadOnly() );
            }
            if(SofaLinkType == item->m_type){
                BaseLink* link = (BaseLink*) item->m_data.value<void*>();
                if( link == nullptr )
                    std::cout << "nUll ptr" << std::endl ;
                return QVariant::fromValue( link->isReadOnly() );
            }
            return QVariant(false);
        case VisibilityRole:
            if(SofaDataType == item->m_type)
            {
                BaseData* data = (BaseData*) item->m_data.value<void*>();
                return QVariant::fromValue( data->isDisplayed() );
            }
            return QVariant(true);
        case RequiredRole:
            if(SofaDataType == item->m_type)
            {
                BaseData* data = (BaseData*) item->m_data.value<void*>();
                return QVariant::fromValue( data->isRequired() );
            }
            return QVariant(true);
        case PathRole:
            if(SofaDataType == item->m_type)
            {
                BaseData* data = (BaseData*) item->m_data.value<void*>();
                return QVariant::fromValue( QString::fromStdString(data->getLinkPath()) );
            }
            else if(SofaLinkType == item->m_type)
            {
                BaseLink* link = (BaseLink*) item->m_data.value<void*>();
                BaseObject* base = link->getOwnerBase()->toBaseObject() ;
                if(!base){
                    dmsg_error("SofaInspectorDataListModel") << "The link seems to be a data link. " ;
                    return QVariant();
                }
                QString fullpath=QString::fromStdString( base->getPathName() + "." +link->getName() ) ;
                return QVariant::fromValue( fullpath );
            }
            return QVariant::fromValue( item->m_name );
        case ValueRole:
            if(SofaDataType == item->m_type)
            {
                BaseData* data = (BaseData*) item->m_data.value<void*>();
                return QVariant::fromValue(new SofaData(data)); //SofaScene::dataValue(data));
            }
            else if(SofaLinkType == item->m_type)
            {
                BaseLink* link = (BaseLink*) item->m_data.value<void*>();
                return QVariant::fromValue(new SofaLink(link)); //SofaScene::linkValue(link));
            }
            else if(LogType == item->m_type)
            {
                return item->m_data; // QVariant::fromValue(item->m_data.toString());
            }
            else if(InfoType == item->m_type)
            {
                return item->m_data; //QVariant::fromValue(item->m_data.toString());
            }
        default:
            break;
        }
    }
    dmsg_error("SofaInspectorDataListModel") << "undefined role";
    return QVariant() ;
}

QHash<int,QByteArray> SofaInspectorDataListModel::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[NameRole]         = "name" ;
    roles[TypeRole]         = "type" ;
    roles[ValueRole]        = "value" ;
    roles[PathRole]         = "path" ;
    roles[ReadOnlyRole]     = "isReadOnly" ;
    roles[VisibilityRole]   = "isDisplayed" ;
    roles[RequiredRole]     = "isRequired" ;
    return roles;
}

SofaData* SofaInspectorDataListModel::getDataById(int parent, int child) const
{
    if(parent < 0 || child < 0)
        return nullptr ;

    if( parent >= m_groups.size() )
        return nullptr;

    if( child >= m_groups[parent]->m_children.size() )
        return nullptr;


    Item* item=m_groups[parent]->m_children[child];
    const QVariant& data = item->m_data ;
    if(item->m_type == SofaDataType)
    {
        return new SofaData(static_cast<BaseData*>(data.value<void*>()));
    }
    return nullptr ;
}

bool SofaInspectorDataListModel::isGroupVisible(int id) const
{
    if(id < 0)
        return true;

    ItemBase* item=m_groups[id] ;
    if(!item)
        msg_fatal("SofaInspectorDataListModel") << "There is no item at this index. " ;

    return item->isVisible() ;
}

bool SofaInspectorDataListModel::isItemVisible(int parent, int child) const
{
    if(parent<0 || child <0)
        return true;

    ItemBase* item=m_groups[parent]->m_children[child];
    return item->isVisible() ;
}

}

