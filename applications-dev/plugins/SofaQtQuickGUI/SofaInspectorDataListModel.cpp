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

#include <QSettings>

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::simulation;

SofaInspectorDataListModel::SofaInspectorDataListModel(QObject* parent) : QAbstractItemModel(parent),
    m_groups(),
    m_selectedsofacomponent(0)
{}

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
    msg_info("SofaInspectorDataListModel") << settings.fileName().toStdString();
    std::cout << "UPDATE THE MODEL to: " << m_selectedsofacomponent << std::endl ;
    if(m_selectedsofacomponent)
    {


        // TODO(dmarchal): delete memory !
        for(auto& group : m_groups)
            delete group;
        m_groups.clear();
        m_nameindex.clear();
        const Base* base = m_selectedsofacomponent->base();
        if(base)
        {
            // SofaData
            QString baseString("Base");
            for(BaseData* data : base->getDataFields())
            {
                const char* gname = data->getGroup() ;
                if(strlen(gname)==0){
                    gname = data->getOwnerClass() ;
                }
                ItemGroup* ig = findOrCreateGroup(getGroupName(QString(gname),
                                                               baseString)) ;
                ig->m_children.append(new Item(QString::fromStdString(data->getName()),
                                               QVariant::fromValue((void*)data),
                                               SofaDataType,
                                               settings.value("inspector/"+ig->m_name+"/"+QString::fromStdString(data->getName()), true).toBool()
                                               ));

            }

            // SofaLink
            std::cout << "LINK UPDATE: " << m_selectedsofacomponent << std::endl ;

            ItemGroup* links = findOrCreateGroup("Links") ;
            for(BaseLink* link : base->getLinks()){
                links->m_children.append(new Item(QString::fromStdString(link->getName()),
                                                  QVariant::fromValue(QString::fromStdString(link->getLinkedPath())),
                                                  SofaLinkType, true));
            }

            // Logs & Warnings
            std::cout << "MESSAGE UPDATE: " << m_selectedsofacomponent << std::endl ;

            QString logName("Messages");
            ItemGroup* logs = findOrCreateGroup(logName) ;
            logs->m_children.append(new Item("info",
                                             QString::fromStdString(base->getOutputs()), LogType, true));
            logs->m_children.append(new Item("warning",
                                             QString::fromStdString(base->getWarnings()), LogType, true));

            // Info
            std::cout << "INFO UPDATE: " << m_selectedsofacomponent << std::endl ;

            QString infoGroup = "Infos";
            ItemGroup* infos = findOrCreateGroup(infoGroup);
            infos->m_children.append(new Item("name",
                                              QString::fromStdString(base->getName()), InfoType, true));
            infos->m_children.append(new Item("class",
                                              QString::fromStdString(base->getClassName()), InfoType, true));

            std::cout << "VISIBILITY UPDATE: " << m_selectedsofacomponent << std::endl ;

            // Retrieve the group visilibity from the settings.
            for(ItemGroup* group : m_groups){
                group->setVisibility(settings.value("inspector/"+group->m_name, true).toBool() );
                group->m_order = settings.value("inspector/ordering/"+group->m_name, 0).toInt() ;
            }

            std::cout << "SORTING: " << m_selectedsofacomponent << std::endl ;

            // Sort the entries using the qStableSort function and a lambda comparator
            // function. Lambdas are C++x11 feature.
            qStableSort(m_groups.begin(), m_groups.end(),
                        [](const ItemGroup* a, const ItemGroup* b) {return a->m_order<b->m_order;});
            std::cout << "END: " << m_selectedsofacomponent << std::endl ;


            for(auto& group : m_groups){
                auto& items = group->m_children;
                qStableSort(items.begin(), items.end(),
                        [](const Item* a, const Item* b) {return a->m_name<b->m_name;});
            }

        }

    }
    std::cout << "END UPDATE THE MODEL" << std::endl ;
}

void SofaInspectorDataListModel::setSofaSelectedComponent(SofaComponent* newSofaComponent)
{
    if(newSofaComponent == m_selectedsofacomponent)
        return;

    if(m_selectedsofacomponent!=nullptr)
        disconnect(m_selectedsofacomponent, SIGNAL(dataHasChanged()),
                   this, SLOT(handleDataHasChanged()));

    m_selectedsofacomponent = newSofaComponent;

    if(newSofaComponent == nullptr)
        return ;

    connect(m_selectedsofacomponent, SIGNAL(dataHasChanged()),
            this, SLOT(handleDataHasChanged()));

    beginResetModel();
    update();
    //sofaSelectedComponentChanged(newSofaComponent);
    endResetModel();
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
    std::cout << "parent:  "<< std::endl;
    return index.parent() ;
}

QModelIndex SofaInspectorDataListModel::index (int row, int column,
                                               const QModelIndex& parent) const
{
    std::cout << "index with: "<<  row << ", " << column << " and " << parent.isValid() <<  std::endl ;

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
{    std::cout << "rowCount for index " << index <<  std::endl ;

    if(index < 0)
        return m_groups.size();
    return m_groups[index]->m_children.size() ;
}

void SofaInspectorDataListModel::setVisibility(int groupid, bool visibility)
{
    std::cout << "setVisibility" <<  groupid << std::endl ;

    if(groupid<0)
        return;

    msg_info("DAMIEN") << "setting visilibity to " << groupid << visibility ;

    QSettings settings;
    settings.setValue("inspector/"+m_groups[groupid]->m_name, visibility);
    m_groups[groupid]->setVisibility(visibility);
}

void SofaInspectorDataListModel::setVisibility(int groupid, int itemid, bool visibility)
{
    std::cout << "setVisibility" <<  groupid << ", " << itemid << std::endl ;

    if(groupid<0 || itemid<0)
        return;
    msg_info("DAMIEN") << "setting visilibity to " << visibility ;
    QSettings settings;
    settings.setValue("inspector/"+m_groups[groupid]->m_name+"/"+m_groups[groupid]->m_children[itemid]->m_name, visibility);
    m_groups[groupid]->m_children[itemid]->setVisibility(visibility);
}

void SofaInspectorDataListModel::resortGroup(int groupid)
{
    auto& items = m_groups[groupid]->m_children;

    emit layoutAboutToBeChanged() ;
    // Sort the entries using the qStableSort function and a lambda comparator
    // function. Lambdas are C++x11 feature.
    qStableSort(items.begin(), items.end(),
            [](const Item* a, const Item* b) {return a->m_visible>b->m_visible;});
    emit layoutChanged();
}

void SofaInspectorDataListModel::setOrdering(int groupid, int idx)
{
    std::cout << "setOrdering" <<  groupid <<  std::endl ;

    QSettings settings;
    settings.setValue("inspector/ordering/"+m_groups[groupid]->m_name, idx);
    std::cout << "SETTINGS... " << std::endl;
    beginResetModel();
    m_groups[groupid]->m_order += idx ;
    qStableSort(m_groups.begin(), m_groups.end(),
                [](const ItemGroup* a, const ItemGroup* b) {return a->m_order<b->m_order;});
    endResetModel();
}

int	SofaInspectorDataListModel::rowCount(const QModelIndex& index) const
{
    std::cout << "rowCount " << index.isValid() <<  std::endl ;

    if(!index.isValid()){
        std::cout << "rowCount  groupe size: " << m_groups.size() << std::endl ;
        return m_groups.size() ;
    }

    ItemGroup* ig=dynamic_cast<ItemGroup*>(static_cast<ItemBase*>(index.internalPointer()));
    if(ig){
        return ig->m_children.size();
    }
    return 0;
}

QVariant SofaInspectorDataListModel::data(const QModelIndex& index, int role) const
{
    std::cout << "data ?(" << index.row() << "," << index.column() << ")" <<  std::endl ;

    // Returns the root of the hierarchy
    if(!index.isValid()){
        return QVariant("");
    }

    if(index.internalPointer() == nullptr){
        dmsg_error("SofaInspectorDataListModel") << "invalid operation";
        return QVariant() ;
    }

    ItemBase* ib=static_cast<ItemBase*>(index.internalPointer());
    std::cout << "dataGroup ?(" << index.row() << "," << index.column() << ", "<< (void*)ib << ")" <<  std::endl ;
    //std::cout << " name: " << ib->m_name.toStdString() << std::endl;
    ItemGroup* ig=dynamic_cast<ItemGroup*>(ib);
    std::cout << "dataGroup !(" << index.row() << "," << index.column() << ")" <<  std::endl ;

    if(ig){
        std::cout << "---> (" << index.row() << "," << index.column() << ")" <<  std::endl ;

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
    std::cout << "dataItem ?(" << index.row() << "," << index.column() << ")" <<  std::endl ;

    Item* item = static_cast<Item*>(index.internalPointer());
    if(item){
        switch(role){
        case NameRole:
            return  QVariant::fromValue(item->m_name);
        case TypeRole:
            return QVariant(item->m_type);
        case ValueRole:
            if(SofaDataType == item->m_type)
            {
                BaseData* data = (BaseData*) item->m_data.value<void*>();
                return QVariant::fromValue(SofaScene::dataValue(data));
            }
            else if(SofaLinkType == item->m_type)
            {
                //BaseLink* link = (BaseLink*) item->m_data.value<void*>();
                return item->m_data; // QVariant::fromValue(SofaScene::linkValue(link));
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

    return roles;
}

SofaData* SofaInspectorDataListModel::getDataById(int parent, int child) const
{
    std::cout << "getDataByid: " <<  parent << ", " << child << std::endl ;

    if(parent < 0 || child < 0)
        return nullptr ;

    Item* item=m_groups[parent]->m_children[child];
    const QVariant& data = item->m_data ;
    if(item->m_type == SofaDataType){
        return new SofaData(m_selectedsofacomponent,
                            (BaseData*)data.value<void*>());
    }
    return nullptr ;
}

bool SofaInspectorDataListModel::isGroupVisible(int id) const
{
    if(id < 0)
        return true;

    std::cout << "isItemvisible" <<  id <<  std::endl ;

    ItemBase* item=m_groups[id] ;
    if(!item)
        msg_fatal("SofaInspectorDataListModel") << "There is no item at this index. " ;

    return item->isVisible() ;
}

bool SofaInspectorDataListModel::isItemVisible(int parent, int child) const
{
    if(parent<0 || child <0)
        return true;
    std::cout << "isItemvisible" <<  parent <<  std::endl ;

    ItemBase* item=m_groups[parent]->m_children[child];
    return item->isVisible() ;
}

}

}

