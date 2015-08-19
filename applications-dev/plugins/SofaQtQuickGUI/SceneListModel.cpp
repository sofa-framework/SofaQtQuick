#include "SceneListModel.h"

#include <QStack>
#include <QDebug>
#include <iostream>

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::simulation;

SceneListModel::SceneListModel(QObject* parent) : QAbstractListModel(parent), MutationListener(),
    myItems(),
    myUpdatedCount(0),
	myIsDirty(true),
    myScene(nullptr)
{

}

SceneListModel::~SceneListModel()
{

}

void SceneListModel::update()
{
	if(!myIsDirty)
		return;

    int changeNum = myItems.size() - myUpdatedCount;
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

    dataChanged(createIndex(0, 0), createIndex(myItems.size() - 1, 0));

    myUpdatedCount = myItems.size();

	myIsDirty = false;
}

void SceneListModel::handleSceneChange(Scene* /*newScene*/)
{
    clear();
    if(myScene)
    {
        if(myScene->isReady())
        {
            //addChild(0, myScene->sofaSimulation()->GetRoot().get());
            update();
        }

        connect(myScene, &Scene::loaded, [this]() {clear(); addChild(0, myScene->sofaSimulation()->GetRoot().get()); update();});
        connect(myScene, &Scene::aboutToUnload, this, &SceneListModel::clear);
    }
}

void SceneListModel::clear()
{
    myItems.clear();

    markDirty();
    update();
}

SceneListModel::Item SceneListModel::buildNodeItem(SceneListModel::Item* parent, BaseNode* node) const
{
    SceneListModel::Item item;

    item.parent = parent;
    item.multiparent = node->getParents().size() > 1;
    item.depth = parent ? parent->depth + 1 : 0;
    item.visibility = !parent ? Visible : (((Hidden | Collapsed) & parent->visibility) ? Hidden : Visible);
    item.base = node;
    item.object = 0;
    item.node = node;

    return item;
}

SceneListModel::Item SceneListModel::buildObjectItem(SceneListModel::Item* parent, BaseObject* object) const
{
    SceneListModel::Item item;

    item.parent = parent;
    item.multiparent = false;
    item.depth = parent ? parent->depth + 1 : 0;
    item.visibility = !parent ? Visible : (((Hidden | Collapsed) & parent->visibility) ? Hidden : Visible);
    item.base = object;
    item.object = object;
    item.node = parent->node;

    return item;
}

void SceneListModel::setScene(Scene* newScene)
{
    if(newScene == myScene)
        return;

    if(myScene)
        myScene->disconnect(this);

    myScene = newScene;

    handleSceneChange(myScene);

    sceneChanged(newScene);
}

int	SceneListModel::rowCount(const QModelIndex & /*parent*/) const
{
    return myItems.size();
}

QVariant SceneListModel::data(const QModelIndex& index, int role) const
{
    if(!myScene || !myScene->isReady())
        return QVariant("");

    if(!index.isValid())
    {
        qWarning("Invalid index");
        return QVariant("");
    }

    if(index.row() >= myItems.size())
    {
        qWarning("Index out of bound");
        return QVariant("");
    }

    const Item& item = myItems[index.row()];
    bool multiparent = item.multiparent;
    int depth = item.depth;
    int visibility = item.visibility;
    Base* base = item.base;
    BaseObject* object = item.object;

    if(0 == base)
    {
        qWarning("Item is empty");
        return QVariant("");
    }

    switch(role)
    {
    case NameRole:
        return QVariant::fromValue(QString(base->name.getValue().c_str()));
    case MultiParentRole:
        return QVariant::fromValue(multiparent);
    case DepthRole:
        return QVariant::fromValue(depth);
    case VisibilityRole:
        return QVariant::fromValue(visibility);
    case TypeRole:
        return QVariant::fromValue(QString(base->getClass()->className.c_str()));
    case IsNodeRole:
        return QVariant::fromValue(0 == object);
    case CollapsibleRole:
        return QVariant::fromValue(0 == object && item.children.size() != 0);
    default:
        qWarning("Role unknown");
    }

    return QVariant("");
}

QHash<int,QByteArray> SceneListModel::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[NameRole]         = "name";
    roles[MultiParentRole]  = "multiparent";
    roles[DepthRole]        = "depth";
    roles[VisibilityRole]   = "visibility";
    roles[TypeRole]         = "type";
    roles[IsNodeRole]       = "isNode";
    roles[CollapsibleRole]  = "collapsible";

    return roles;
}

QVariant SceneListModel::get(int row) const
{
    QVariantMap object;

    if(-1 == row)
    {
        qWarning("Invalid index");
        return object;
    }

    if(row >= myItems.size())
        return object;

    QHash<int,QByteArray> roles = roleNames();
    for(auto roleIt = roles.begin(); roleIt != roles.end(); ++roleIt)
        object.insert(roleIt.value(), data(createIndex(row, 0), roleIt.key()));

    return object;
}

SceneComponent* SceneListModel::getComponentById(int row) const
{
    if(row < 0 || row >= myItems.size())
        return 0;

    return new SceneComponent(myScene, myItems.at(row).base);
}

void SceneListModel::setCollapsed(int row, bool collapsed)
{
    if(-1 == row)
    {
        qWarning("Invalid index");
        return;
    }

    if(row >= myItems.size())
        return;

    Item& item = myItems[row];

    int collapsedFlag = collapsed ? Collapsed : Visible;
    if(collapsedFlag == item.visibility)
        return;

    item.visibility = collapsed;

    QStack<Item*> children;
    for(int i = 0; i < item.children.size(); ++i)
        children.append(item.children[i]);

    while(!children.isEmpty())
    {
        Item* child = children.pop();
        if(1 == collapsed)
            child->visibility |= Hidden;
        else
            child->visibility ^= Hidden;

        if(!(Collapsed & child->visibility))
            for(int i = 0; i < child->children.size(); ++i)
                children.append(child->children[i]);
    }

    markDirty();
    update();
}

bool SceneListModel::isAncestor(SceneListModel::Item* ancestor, SceneListModel::Item* child)
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

void SceneListModel::addChild(Node* parent, Node* child)
{
    if(!child)
        return;

    //std::cerr << "++" << child->getName().c_str() << "(" << child << ")" << " ; " << (parent ? parent->getName().c_str() : "") << "(" << parent << ")" << " > " << std::flush;

    if(!parent)
    {
        myItems.append(buildNodeItem(0, child));
    }
    else
    {
        bool alreadyAdded = false;

        QList<Item>::iterator parentItemIt = myItems.begin();
        while(parentItemIt != myItems.end())
        {
            if(parent == parentItemIt->base)
            {
                Item* parentItem = &*parentItemIt;

                QList<Item>::iterator itemIt = parentItemIt;
                while(++itemIt != myItems.end())
                {
                    Item* childItem = &*itemIt;
                    if(childItem->base == child)
                    {
                        alreadyAdded = true;
                        break;
                    }

                    if(!isAncestor(parentItem, childItem))
                        break;
                }

                if(alreadyAdded)
                    break;

                QList<Item>::iterator childItemIt = myItems.insert(itemIt, buildNodeItem(parentItem, child));
                //std::cerr << &*childItemIt << std::endl;
                parentItem->children.append(&*childItemIt);

                parentItemIt = childItemIt;
                break;
            }
            else
                ++parentItemIt;
        }
    }

    MutationListener::addChild(parent, child);

	markDirty();
}

void SceneListModel::removeChild(Node* parent, Node* child)
{
    if(!child)
        return;

    MutationListener::removeChild(parent, child);

    //std::cerr << "--" << child->getName().c_str() << "(" << child << ")" << " ; " << (parent ? parent->getName().c_str() : "") << "(" << parent << ")" << " > " << std::flush;

    QList<Item>::iterator itemIt = myItems.begin();
    while(itemIt != myItems.end())
    {
        if(child == itemIt->base)
        {
            Item* parentItem = itemIt->parent;
            if((!parent && !parentItem) || (parent && parentItem && parent == parentItem->base))
            {
                //std::cerr << &*itemIt << std::endl;

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

	markDirty();
}

//void SceneListModel::moveChild(Node* previous, Node* parent, Node* child)
//{

//}

void SceneListModel::addObject(Node* parent, BaseObject* object)
{
    if(!object || !parent)
        return;

    //std::cerr << "+" << object->getName().c_str() << "(" << object << ")" << " ; " << (parent ? parent->getName().c_str() : "") << "(" << parent << ")" << " > " << std::flush;

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
                    if(parentItem != itemIt->parent)
                        break;

                QList<Item>::iterator childItemIt = myItems.insert(itemIt, buildObjectItem(parentItem, object));
                parentItem->children.append(&*childItemIt);
                parentItemIt = childItemIt;

                break;
            }
            else
                ++parentItemIt;
        }

        markDirty();
    }

    MutationListener::addObject(parent, object);
}

void SceneListModel::removeObject(Node* parent, BaseObject* object)
{
    if(!object || !parent)
        return;

    MutationListener::removeObject(parent, object);

    //std::cerr << "-" << object->getName().c_str() << "(" << object << ")" << " ; " << (parent ? parent->getName().c_str() : "") << "(" << parent << ")" << " > " << std::flush;

    QList<Item>::iterator itemIt = myItems.begin();
    while(itemIt != myItems.end())
    {
        if(object == itemIt->base)
        {
            Item* parentItem = itemIt->parent;
            if(parentItem && parent == parentItem->base)
            {
                //std::cerr << &*itemIt << " - " << &*itemIt->parent << std::endl;
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

	markDirty();
}

//void SceneListModel::moveObject(Node* previous, Node* parent, BaseObject* object)
//{
//	markDirty();
//}

void SceneListModel::addSlave(BaseObject* master, BaseObject* slave)
{
    MutationListener::addSlave(master, slave);

	markDirty();
}

void SceneListModel::removeSlave(BaseObject* master, BaseObject* slave)
{
    MutationListener::removeSlave(master, slave);

	markDirty();
}

}

}
