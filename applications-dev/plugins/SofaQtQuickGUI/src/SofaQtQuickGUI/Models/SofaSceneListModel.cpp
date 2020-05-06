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

#include "SofaSceneListModel.h"

#include <QStack>
#include <iostream>
#include <algorithm>

namespace sofaqtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::simulation;

using sofa::qtquick::SofaComponent;

SofaSceneListModel::SofaSceneListModel(QObject* parent) : QAbstractListModel(parent), MutationListener(),
    myItems(),
    mySofaScene(nullptr)
{

}

SofaSceneListModel::~SofaSceneListModel()
{
    clear();
}

void SofaSceneListModel::update()
{
    dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, 0));
}

void SofaSceneListModel::handleSceneChange(SofaBaseScene*)
{
    clear();
    if(mySofaScene)
    {
        if(mySofaScene->isReady())
        {
            mySofaScene->sofaRootNode()->addListener(this);
            onBeginAddChild(nullptr, mySofaScene->sofaRootNode().get());
            onEndAddChild(nullptr, mySofaScene->sofaRootNode().get());
        }

        connect(mySofaScene, &SofaBaseScene::statusChanged, this, [this]() {
            clear();

            if(SofaBaseScene::Ready == mySofaScene->status())
            {
                mySofaScene->sofaRootNode()->addListener(this);
                onBeginAddChild(nullptr, mySofaScene->sofaRootNode().get());
                onEndAddChild(nullptr, mySofaScene->sofaRootNode().get());
            }
        });
    }
}


void SofaSceneListModel::clear()
{
    if(myItems.isEmpty())
        return;

    for(Item& item : myItems)
        if(item.base == item.node)
        {
            Node* node = static_cast<Node*>(item.node);
            if(node)
                node->removeListener(this);
        }

    beginRemoveRows(QModelIndex(), 0, myItems.size() - 1);

    myItems.clear();

    endRemoveRows();
}

SofaSceneListModel::Item SofaSceneListModel::buildNodeItem(SofaSceneListModel::Item* parent, BaseNode* node) const
{
    SofaSceneListModel::Item item;

    item.parent = parent;
    item.multiparent = node->getNbParents() > 1;
    item.firstparent = node->getNbParents()>0? node->getParents()[0] == parent->node : true ;
    item.depth = parent ? parent->depth + 1 : 0;
    item.base = node;
    item.object = 0;
    item.node = node;

    return item;
}

SofaSceneListModel::Item SofaSceneListModel::buildObjectItem(SofaSceneListModel::Item* parent, BaseObject* object) const
{
    SofaSceneListModel::Item item;

    item.parent = parent;
    item.multiparent = false;
    item.depth = parent ? parent->depth + 1 : 0;
    item.base = object;
    item.object = object;
    item.node = parent->node;

    return item;
}

void SofaSceneListModel::setSofaScene(SofaBaseScene* newSofaScene)
{
    if(newSofaScene == mySofaScene)
        return;

    if(mySofaScene)
        disconnect(mySofaScene);

    mySofaScene = newSofaScene;

    handleSceneChange(mySofaScene);
    sofaSceneChanged(newSofaScene);
}

int SofaSceneListModel::computeModelRow(int itemRow) const
{
    if(itemRow >= 0 && itemRow < myItems.size())
    {
        int currentModelRow = -1;

        int currentItemRow = 0;
        for(const Item& item : myItems)
        {
            SOFA_UNUSED(item);
            ++currentModelRow;

            if(currentItemRow == itemRow)
                return currentModelRow;

            ++currentItemRow;
        }
    }

    return -1;
}

int SofaSceneListModel::computeItemRow(int modelRow) const
{
    if(modelRow >= 0)
    {
        int currentModelRow = -1;
        int currentItemRow = 0;
        for(const Item& item : myItems)
        {
            SOFA_UNUSED(item);
            ++currentModelRow;

            if(currentModelRow == modelRow)
                return currentItemRow;

            ++currentItemRow;
        }
    }

    return -1;
}

int SofaSceneListModel::computeItemRow(const QModelIndex& index) const
{
    if(index.isValid())
        return computeItemRow(index.row());

    return -1;
}

int SofaSceneListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return myItems.size() ;
}

QVariant SofaSceneListModel::data(const QModelIndex& index, int role) const
{
    if(!mySofaScene || !mySofaScene->isReady())
        return QVariant("");

    int row = computeItemRow(index);

    if(-1 == row)
    {
        msg_error("SofaQtQuickGUI") << "Invalid index " << row;
        return QVariant("");
    }

    const Item& item = myItems[row];
    bool multiparent = item.multiparent;
    bool firstparent = item.firstparent;
    int depth = item.depth;
    Base* base = item.base;
    BaseObject* object = item.object;

    if(0 == base)
    {
        msg_error("SofaQtQuickGUI") << "Item is empty";
        return QVariant("");
    }

    switch(role)
    {
    case NameRole:
        return QVariant::fromValue(QString::fromStdString(base->getName()));
    case MultiParentRole:
        return QVariant::fromValue(multiparent);
    case FirstParentRole:
        return QVariant::fromValue(firstparent);
    case DepthRole:
        return QVariant::fromValue(depth);
    case TypeRole:
        return QVariant::fromValue(QString::fromStdString(base->getClass()->className));
    case IsNodeRole:
        return QVariant::fromValue(0 == object);
    default:
        msg_error("SofaQtQuickGUI") << "Role unknown";
    }

    return QVariant("");
}

QHash<int,QByteArray> SofaSceneListModel::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[NameRole]         = "name";
    roles[MultiParentRole]  = "multiparent";
    roles[FirstParentRole]  = "firstparent";
    roles[DepthRole]        = "depth";
    roles[TypeRole]         = "type";
    roles[IsNodeRole]       = "isNode";

    return roles;
}

QVariant SofaSceneListModel::get(int row) const
{
    QVariantMap object;

    if(row < 0 || row > myItems.size())
    {
        msg_error("SofaQtQuickGUI") << "Invalid index " << row;
        return object;
    }

    QHash<int,QByteArray> roles = roleNames();
    for(auto roleIt = roles.begin(); roleIt != roles.end(); ++roleIt)
        object.insert(roleIt.value(), data(createIndex(row, 0), roleIt.key()));

    return object;
}

SofaBase* SofaSceneListModel::getBaseById(int modelRow) const
{
    int itemRow = computeItemRow(modelRow);
    if(-1 == itemRow)
    {
        msg_error("SofaQtQuickGUI") << "Invalid index " << itemRow;
        return nullptr;
    }

    return new SofaBase(myItems.at(itemRow).base);
}


SofaComponent* SofaSceneListModel::getComponentById(int modelRow) const
{
    int itemRow = computeItemRow(modelRow);
    if(-1 == itemRow)
    {
        msg_error("SofaQtQuickGUI") << "Invalid index " << itemRow;
        return nullptr;
    }

    return new SofaComponent(mySofaScene, myItems.at(itemRow).base);
}

int SofaSceneListModel::getComponentId(SofaComponent* sofaComponent) const
{
    Base* base = nullptr;
    if(sofaComponent)
        base = sofaComponent->base();

    if(base)
        for(int i = 0; i < myItems.size(); ++i)
            if(base == myItems[i].base)
                return computeModelRow(i);

    return -1;
}

QList<int> SofaSceneListModel::computeFilteredRows(const QString& filter) const
{
    QList<int> filteredRows;

    std::cout << "filter: " << filter.toStdString() << std::endl;
    std::cout << "items size : " << myItems.size() << std::endl;
    if(!filter.isEmpty())
        for(int i = 0; i < myItems.size(); ++i)
        {
            const Item& item = myItems[i];
            std::cout << item.base->getName() << std::endl;
            QString name = QString::fromStdString(item.base->getName());
            if(-1 != name.indexOf(filter, 0, Qt::CaseInsensitive))
                filteredRows.append(computeModelRow(i));
        }

    return filteredRows;
}

bool SofaSceneListModel::isAncestor(SofaSceneListModel::Item* ancestor, SofaSceneListModel::Item* child)
{
    if(!child)
        return false;

    if(!ancestor)
        return true;

    Item* parent = child;
    while((parent = parent->parent))
        if(ancestor == parent)
            return true;

    return false;
}

unsigned int SofaSceneListModel::countChildrenOf(const SofaSceneListModel::Item& a)
{
    if(a.children.empty())
        return 0;
    unsigned int count = a.children.size() ;
    for(auto c : a.children)
        count += countChildrenOf(*c) ;
    return count;
}

void SofaSceneListModel::onBeginAddChild(Node* parent, Node* child)
{
    if(!child)
        return;

    /// Add a child as a root node.
    if(!parent)
    {
        beginInsertRows(QModelIndex(), 0, 0);
        myItems.append(buildNodeItem(0, child));
        endInsertRows();
        for (auto o : child->object)
        {
            onBeginAddObject(child, o.get());
            onEndAddObject(child, o.get());
        }
        for (auto c : child->getChildren())
        {
            onBeginAddChild(child, static_cast<Node*>(c));
            onEndAddChild(child, static_cast<Node*>(c));
        }
        return;
    }

    for (auto o : child->object)
    {
        onBeginAddObject(child, o.get());
        onEndAddObject(child, o.get());
    }
    for (auto c : child->getChildren())
    {
        onBeginAddChild(child, static_cast<Node*>(c));
        onEndAddChild(child, static_cast<Node*>(c));
    }

    auto parentItemIt = std::find_if(myItems.begin(), myItems.end(), [parent](const Item& a){ return parent == a.base; });
    /// Check if the parent is in the Item list, otherwise we have nothing to do;
    if( parentItemIt == myItems.end() )
    {
        return;
    }
    Item& parentItem = *parentItemIt;

    /// Check if the child is already in the Item list, if this is the case, then we just have nothing to do
    /// because this is indicating the it was already added.
    auto firstChildIt = parentItemIt++ ;
    auto lastChildIt = firstChildIt+parentItem.children.size() ;
    if( std::find_if(firstChildIt, lastChildIt, [child](const Item& a){ return child == a.base; }) != lastChildIt )
    {
        return;
    }

    unsigned int numChildren = countChildrenOf(parentItem) ;

    auto insertIterator = firstChildIt+numChildren+1 ;
    int idx = insertIterator-myItems.begin() ;

    beginInsertRows(QModelIndex(), idx, idx);
    auto newChildItemIt = myItems.insert(insertIterator, buildNodeItem(&parentItem, child));

    parentItem.children.append(&*newChildItemIt);
    endInsertRows();
}

void SofaSceneListModel::onEndRemoveChild(Node* parent, Node* child)
{
    if(!child)
        return;

    QList<Item>::iterator itemIt = myItems.begin();
    while(itemIt != myItems.end())
    {
        if(child == itemIt->base)
        {
            Item* parentItem = itemIt->parent;
            if((!parent && !parentItem) || (parent && parentItem && parent == parentItem->base))
            {
                if(parentItem)
                {
                    int index = parentItem->children.indexOf(&*itemIt);
                    if(-1 != index)
                        parentItem->children.remove(index);
                }

                itemIt = myItems.erase(itemIt);

                break;
            }
            else
                ++itemIt;
        }
        else
        {
            ++itemIt;
        }
    }
}

void SofaSceneListModel::onBeginAddObject(Node* parent, BaseObject* object)
{
    if(!object || !parent)
        return;

    bool alreadyAdded = false;
    for(auto it = myItems.begin(); it != myItems.end(); ++it)
        if(it->base == object)
        {
            alreadyAdded = true;
            break;
        }

    if(!alreadyAdded)
    {
        QList<Item>::iterator parentItemIt = myItems.begin();
        while(parentItemIt != myItems.end())
        {
            if(parent == parentItemIt->base)
            {
                Item* parentItem = &*parentItemIt;

                QList<Item>::iterator itemIt = parentItemIt;
                while(++itemIt != myItems.end())
                    if(parentItem != itemIt->parent || !itemIt->object)
                        break;

                QList<Item>::iterator childItemIt = myItems.insert(itemIt, buildObjectItem(parentItem, object));
                parentItem->children.append(&*childItemIt);
                parentItemIt = childItemIt;

                break;
            }
            else
                ++parentItemIt;
        }
    }
}

void SofaSceneListModel::onEndRemoveObject(Node* parent, BaseObject* object)
{
    if(!object || !parent)
        return;

    QList<Item>::iterator itemIt = myItems.begin();
    while(itemIt != myItems.end())
    {
        if(object == itemIt->base)
        {
            Item* parentItem = itemIt->parent;
            if(parentItem && parent == parentItem->base)
            {
                if(parentItem)
                {
                    int index = parentItem->children.indexOf(&*itemIt);

                    if(-1 != index)
                        parentItem->children.remove(index);
                }

                itemIt = myItems.erase(itemIt);
                break;
            }
            else
                ++itemIt;
        }
        else
        {
            ++itemIt;
        }
    }
}

}  // namespace sofaqtquick
