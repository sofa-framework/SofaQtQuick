/*******************************************************************
Copyright 2018, CNRS

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
********************************************************************/
/*******************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/

/// The order for the inclusion sequence is from the wider to the narrower.
#include <iostream>
#include <algorithm>
#include <functional>

#include <QStack>
#include <sofa/simulation/Visitor.h>
#include "SofaSceneItemModel.h"
#include "SofaSimulationGraph/DAGNode.h"

namespace sofa
{
namespace qtquick
{
namespace _sofasceneitemmodel_
{
using sofa::core::objectmodel::Base;
using sofa::core::objectmodel::BaseObject;
using sofa::core::objectmodel::BaseNode;
using sofa::simulation::Node;

SofaSceneItemModel::SofaSceneItemModel(QObject* parent) : QAbstractItemModel(parent), MutationListener()
{
//    connect(this, &SofaSceneItemModel::resetRequired, this, &SofaSceneItemModel::onResetRequired);
//    this->m_needsRefresh = false;
//    this->m_isRefreshThreadRunning = true;
//    m_thread = std::thread(&SofaSceneItemModel::modelRefreshThread, this);
}

SofaSceneItemModel::~SofaSceneItemModel()
{
    if(m_root!=nullptr)
        m_root->removeListener(this);
//    m_isRefreshThreadRunning = false;
//    m_thread.join();
}

QModelIndex SofaSceneItemModel::index(int row, int column, const QModelIndex &parent) const
{
//    qDebug() << "index query " << parent << " : " << row << ", " << column ;

    sofa::core::objectmodel::Base* currentBase ;
    if (!parent.isValid())
    {
        currentBase=m_scene->sofaRootNode().get();
        if (!row && !column)
        {
//            qDebug() << "index for "<< QString::fromStdString(currentBase->getName()) << " is " << createIndex(0, 0, static_cast<Base*>(currentBase->toBaseNode())) ;
            return createIndex(0, 0, static_cast<Base*>(currentBase->toBaseNode()));
        }
        else
        {
            qDebug() << "index query " << parent << " : " << row << ", " << column << " is invalid" ;
            return QModelIndex();
        }
    }
    currentBase=static_cast<sofa::core::objectmodel::Base*>(parent.internalPointer());

    if(currentBase->toBaseNode())
    {
        QModelIndex newIndex ;
        Node* currentNode = dynamic_cast<Node*>(currentBase);

        /// The row number is encoding either a node or a component.
        /// Its values are between 0 and object.size()+childNode.size().
        /// The components are before the child node so detect if the row is refereing to a
        /// component index or a child node index we are checking if it larger than the components's
        /// container size().
        sofa::core::objectmodel::Base* b;
        if(size_t(row) < currentNode->object.size())
        {
            b=static_cast<sofa::core::objectmodel::Base*>(currentNode->object[unsigned(row)].get());
            newIndex=createIndex(row, 0, b);
            //qDebug() << "index query " << current << " : " << row << ", " << column << " node: "
            //         << QString::fromStdString(currentNode->getName()) << " => object " << newIndex << "  " << currentNode->object[row].get()->toBaseNode() ;
        }
        else
        {
            /// Lets find our parent location.
            /// We change the row id so it fit inside the child array.
            size_t nrow = size_t(row) - currentNode->object.size();
            if(nrow >= currentNode->child.size())
            {
//                qWarning() << "Inavlid row number =>" << row << column << parent;
                return QModelIndex();
            }
            auto childNode = currentNode->child[unsigned(nrow)].get();
            b=childNode;
            int c = (childNode->getNbParents() != 1) ;
            newIndex=createIndex(row, c, b);
            //qDebug() << "index query " << current << " : " << row << ", " << column << " node: "
            //         << QString::fromStdString(currentNode->getName()) << " => node " << newIndex  << "  " << currentNode->child[row].get()->toBaseNode() ;
        }
//        qDebug() << "index for "<< QString::fromStdString(b->getName()) << " is " << newIndex ;

        return newIndex;
    }
//        qDebug() << "index query " << parent << " : " << row << ", " << column << " is invalid" ;

    return QModelIndex(); //createIndex(row, column, nullptr);
}

QModelIndex SofaSceneItemModel::index(Node* node) const
{
//        qDebug() << "index query for node " << QString::fromStdString(node->getName());
    if(node==nullptr)
    {
        return QModelIndex();
    }

    Node* parentNode=static_cast<Node*>(node->getFirstParent());
    if (!parentNode)
        return createIndex(0, 0, static_cast<Base*>(node));

    size_t d = 0;
    d += parentNode->object.size() + size_t(std::distance(parentNode->child.begin(),
                       std::find(parentNode->child.begin(), parentNode->child.end(), node)));

    int c = node->getNbParents()!=1;
    return createIndex(int(d), c, static_cast<sofa::core::objectmodel::Base*>(node));
}

QModelIndex SofaSceneItemModel::index(Node* parent, sofa::core::objectmodel::BaseObject* obj) const
{
//        qDebug() << "Object index query (" << QString::fromStdString(parent->getName()) << ", " << QString::fromStdString(obj->getName()) <<  ")";

    if(obj == nullptr || parent == nullptr)
        return QModelIndex();

    size_t d = size_t(std::distance(parent->object.begin(),
                          std::find(parent->object.begin(), parent->object.end(), obj))) ;
    return createIndex(int(d), 0, static_cast<sofa::core::objectmodel::Base*>(obj));
}

QModelIndex SofaSceneItemModel::parent(const QModelIndex &index) const
{
//    qDebug() << "parent query (" << index << "): " << QString::fromStdString(static_cast<sofa::core::objectmodel::Base*>(index.internalPointer())->getName());
    if(!index.isValid())
        return QModelIndex();

    sofa::core::objectmodel::Base* currentBase = static_cast<sofa::core::objectmodel::Base*>(index.internalPointer()) ;
    if (currentBase && currentBase->toBaseNode())
    {
        Node* currentNode = static_cast<Node*>(currentBase->toBaseNode());
        if (!currentNode || !currentNode->getFirstParent())
            return QModelIndex();

        //qDebug() << "   a: " << QString::fromStdString(currentNode->getName());
        Node* parentNode=static_cast<Node*>(currentNode->getFirstParent());
        Node* grandparentNode = static_cast<Node*>(parentNode->getFirstParent());

        QModelIndex idx;
        if(grandparentNode==nullptr)
        {
            return createIndex(0, 0, static_cast<Base*>(m_scene->sofaRootNode().get()));
        }
        else
        {
            size_t d = size_t(std::distance(grandparentNode->child.begin(),
                              std::find(grandparentNode->child.begin(), grandparentNode->child.end(), parentNode))) ;
            int c = parentNode->getNbParents()!=1;
            idx=createIndex(int(grandparentNode->object.size()+d), c, static_cast<sofa::core::objectmodel::Base*>(parentNode));
        }
        //qDebug() << "   b: " << QString::fromStdString(parentNode->getName()) << " => " << idx;

        return idx;
    }
//    qDebug() << "SofaSceneItemModel::parent: invalid index!" << index;
    return QModelIndex();
}

int SofaSceneItemModel::rowCount(const QModelIndex &index) const
{   
//        qDebug() << "rowCount " << index ;

    if (!index.isValid())
        return 1;
    if(index.column()>0)
        return 0;
    sofa::core::objectmodel::Base* currentBase=static_cast<sofa::core::objectmodel::Base*>(index.internalPointer());

    if(currentBase->toBaseNode())
    {
        Node* currentNode = static_cast<Node*>(currentBase->toBaseNode());
        size_t numRows = currentNode->child.size() + currentNode->object.size();

//                qDebug() << "     " << index << " node: " << QString::fromStdString(currentNode->getName())
//                            << "  => " << numRows;

        return int(numRows);
    }

    return 0;
}

bool SofaSceneItemModel::hasChildren(const QModelIndex &index) const
{
    /// A column higher than 0 encode the fact the index has multiple parent.
    /// We don't want to duplicate its content (because this crash TreeView)
    /// so we force it to report zero children.
    if(index.column()>0)
        return false;
    return (rowCount(index) > 0);

//    sofa::core::objectmodel::Base* currentBase ;
//    if (!index.isValid()){
//        return true;
//    }else{
//        currentBase=static_cast<sofa::core::objectmodel::Base*>(index.internalPointer());
//    }

//    /// Test if we are a not and not a baseobject.
//    sofa::core::objectmodel::BaseNode* basenode = currentBase->toBaseNode();
//    if(!basenode)
//        return false;

//    Node* node = static_cast<Node*>(basenode);
//    return !node->child.empty() || !node->object.empty();
}

int SofaSceneItemModel::columnCount(const QModelIndex &index) const
{
    SOFA_UNUSED(index);
    return 1;
}

sofa::qtquick::SofaComponent* SofaSceneItemModel::getComponentFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return nullptr;

    sofa::core::objectmodel::Base* currentBase=static_cast<sofa::core::objectmodel::Base*>(index.internalPointer());

    if(!currentBase)
        return nullptr;

    return new SofaComponent(m_scene, currentBase);
}

QModelIndex SofaSceneItemModel::getIndexFromComponent(sofa::qtquick::SofaComponent* component) const
{
    if (component)
    {
        sofa::simulation::Node* node=dynamic_cast<sofa::simulation::Node*>(component->base());
        if(node)
            return index(node);

        sofa::core::objectmodel::BaseObject* obj = dynamic_cast<sofa::core::objectmodel::BaseObject*>(component->base());
        if(obj)
            return index(static_cast<Node*>(obj->getContext()->toBaseNode()), obj);
    }
    return QModelIndex();
}

QVariant SofaSceneItemModel::data(const QModelIndex &index, int role) const
{    
//        qDebug() << "data: " << index << "role " << role ;
    sofa::core::objectmodel::Base* currentBase;
    sofa::core::objectmodel::BaseNode* currentNode;

    if (!index.isValid()){
        return false;
    }else{
        currentBase=static_cast<sofa::core::objectmodel::Base*>(index.internalPointer());
    }

    currentNode = currentBase->toBaseNode();

    switch(static_cast<Roles>(role))
    {
    case Roles::Name:
        return QVariant(QString::fromStdString(currentBase->getName()));
    case Roles::TypeName:
        return QVariant::fromValue(QString::fromStdString(currentBase->getClass()->className));
    case Roles::IsNode:
        return QVariant(currentBase->toBaseNode()!=nullptr);
    case Roles::IsMultiParent:
        if(currentNode)
            return QVariant(currentBase->toBaseNode()->getNbParents()!=1);
        else
            return QVariant(false);
    case Roles::HasMultiParent:
        if(currentNode)
            return QVariant(index.column()>0);
        else
            return QVariant(false);
    case Roles::IsEnabled:
        return QVariant(true);
    case Roles::Row:
        return QVariant(index.row());
    }

    //qDebug() << "data: " << index << "role " << role ;
    return QVariant(QString("damien"));
}

QHash<int, QByteArray> SofaSceneItemModel::roleNames() const
{
    /// Create a static mapping to link the "role" number as Qt is using and the corresponding
    /// textual represresentation; The Roles are encoded using an 'enum class' so they don't
    /// automatically convert to int value in the constructor initializer_list.
    static QHash<int, QByteArray> mapping {{
            { int(Roles::Name), "name" },
            { int(Roles::TypeName), "typename" },
            { int(Roles::IsNode), "isNode" },
            { int(Roles::IsMultiParent), "isMultiParent" },
            { int(Roles::HasMultiParent), "hasMultiParent" },
            { int(Roles::IsEnabled), "isEnabled" },
            { int(Roles::Row), "row" }
                                           }};
    return mapping;
}

size_t rrowCount(Node* parent)
{
    if (parent == nullptr)
        return 0;
    return parent->child.size() + parent->object.size() ;
}

size_t rrowCount_recurse(Node* node)
{
    if (node == nullptr)
        return 0;
    size_t i = 0;
    for (Node::SPtr child: node->child)
        i += 1 + rrowCount_recurse(child.get());
    return i + node->object.size() ;
}

void SofaSceneItemModel::modelRefreshThread()
{
    clock_t startTime = clock();

    while(m_isRefreshThreadRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        clock_t testTime = clock();
        clock_t timePassed = testTime - startTime;
        double secondsPassed = timePassed / double(CLOCKS_PER_SEC);
        if (secondsPassed >= 0.2)
        {
//            std::cout << ".2 seconds passed" << std::endl;
            startTime = testTime;
            if (this->m_needsRefresh)
                emit resetRequired();
        }
    }
}

void SofaSceneItemModel::onResetRequired()
{
//    std::cout << "reset required" << std::endl;
    beginResetModel();
    endResetModel();
    emit modelHasReset();
    this->m_needsRefresh = false;
}


void SofaSceneItemModel::onAddChildBegin(Node* target, Node* child, unsigned idx)
{
//    return;
    SOFA_UNUSED(child);
    QModelIndex parentIndex = index(target);
    int i = int(rrowCount(target));
    if (target != nullptr)
        i = int(target->object.size() + idx);

//    msg_info("b") << "=========== Adding child node " << child->getName() << " to: " << ((target) ? target->getName() : "NULL") << " (" << parentIndex.row() << ")";
//    msg_warning("       ") << target->getName() << " row is: " << parentIndex.row();
//    msg_warning("       ") << child->getName() <<  " location in parent is: " << i;
//    msg_warning("       ") << "nb of additional rows to add: " << int(rrowCount_recurse(child));

//    qDebug() << "parent index: " << parentIndex;

//    if (child->child.size())
//    {
//        msg_info("Pointer Values: ") << "New node: " << child << " " << child->getName() << "\nParent: " << target << " " << target->getName()  << "\nRoot: " << m_root.get() << " " << m_root->getName() ;
//    }
//    // only adding the node. subnodes & objects are added after (onAddChildEnd)
//    std::cout << "adding 1 row for " << child->getName() << " in " << target->getName() << std::endl;
    beginInsertRows(parentIndex, i, i);
}

void SofaSceneItemModel::addNodeContent(Node* node)
{
    QModelIndex idx = index(node);
    // adding as many rows as there are components in child
    beginInsertRows(idx, 0, int(node->object.size()));
    endInsertRows();

    int i = int(node->object.size());
    for (Node::SPtr child : node->child)
    {
        beginInsertRows(idx, i, i);
        endInsertRows();
        addNodeContent(child.get());
        i++;
    }
}

void SofaSceneItemModel::onAddChildEnd(Node* target, Node* child, unsigned index)
{
//    m_needsRefresh = true;
//    return;
    SOFA_UNUSED(target);
    SOFA_UNUSED(child);
    SOFA_UNUSED(index);

    endInsertRows();

    // call onAddChildBegin / onAddChildEnd for each sub nodes
    for (Node::SPtr n : child->child)
    {
        addNodeContent(n.get());
    }
//    qDebug() << "child index: " << index(child);
//    msg_info("e") << "========== " << child->getName() << "added at row" << index(child).row() << " ==========";
}

// because indexes to be removed must have their actual parent,
// recursive removal of a node's childs must be done with this function
void SofaSceneItemModel::removeNodeContent(Node* node)
{
    QModelIndex idx = index(node);

    // Remove the content of each subnode
    for (Node::SPtr child : node->child)
        removeNodeContent(child.get());

    // Remove 1st level content of the current node
//    std::cout << "removing " << rrowCount(node) << " rows from " << node->getName() << std::endl;
    beginRemoveRows(idx, 0, int(rrowCount(node)));
    endRemoveRows(); // we end the removal immediately
}


void SofaSceneItemModel::onRemoveChildBegin(Node* parent, Node* child)
{
//    return;
////    msg_info("b") << "======== Removing child node " << child->getName() << " from: " << parent->getName();

    QModelIndex parentIndex = index(parent);
    int childIndex = index(child).row();

//    msg_info("b") << "=========== Removing child node " << child->getName() << " from: " << ((parent) ? parent->getName() : "NULL") << " (" << parentIndex.row() << ")";
//    qDebug() << "  parent location is: " << parentIndex.row();
//    qDebug() << "       child location is: " << childIndex;
//    qDebug() << "       nb of additional rows to remove: " << int(rrowCount_recurse(child)) ;

    removeNodeContent(child);
    beginRemoveRows(parentIndex, childIndex, childIndex /*+ int(rrowCount_recurse(child))*/);
}

void SofaSceneItemModel::onRemoveChildEnd(Node* parent, Node* child)
{
//    m_needsRefresh = true;
//    return;
    SOFA_UNUSED(child);
    SOFA_UNUSED(parent);
    endRemoveRows();
////    msg_info("e") << "========== " << child->getName() << "removed ==========";
}



void SofaSceneItemModel::onAddObjectBegin(Node* parent, core::objectmodel::BaseObject* obj)
{
//    return;
    SOFA_UNUSED(obj);
//    msg_info("b") << "============= Adding object " << obj->getName() << " to: " << parent->getName();
    QModelIndex parentIndex = index(parent);
    int objIndex = int(parent->object.size());


//    qDebug() << "  parent location is: " << parentIndex.row();
//    qDebug() << "       child location is: " << objIndex;

    beginInsertRows(parentIndex, objIndex, objIndex);
}
void SofaSceneItemModel::onAddObjectEnd(Node* parent, core::objectmodel::BaseObject* obj)
{
//    m_needsRefresh = true;
//    return;
    SOFA_UNUSED(obj);
    SOFA_UNUSED(parent);
    endInsertRows();
////    msg_info("b") << "========== Adding object done: " << obj->getName();
}
void SofaSceneItemModel::onRemoveObjectBegin(Node* parent, core::objectmodel::BaseObject* obj)
{
//    return;
//    msg_info("b") << "============= Removing object " << obj->getName() << " from: " << parent->getName();
    QModelIndex parentIndex = index(parent);
    int objIndex = index(parent, obj).row();

//    qDebug() << "  parent location is: " << parentIndex.row();
//    qDebug() << "       child location is: " << objIndex ;

    beginRemoveRows(parentIndex, objIndex, objIndex);
}
void SofaSceneItemModel::onRemoveObjectEnd(Node* parent, core::objectmodel::BaseObject* obj)
{
//    m_needsRefresh = true;
//    return;
    SOFA_UNUSED(obj);
    SOFA_UNUSED(parent);
    endRemoveRows();
////    msg_info("b") << "========== Removing object done: " << obj->getName();
}


SofaScene* SofaSceneItemModel::sofaScene() const
{
    return m_scene;
}

void SofaSceneItemModel::setSofaScene(SofaScene* newScene)
{    
    m_scene = newScene;

    /// The scene passed to this model is tracked to monitor if its status has changed.
    /// If this is the case then the model needs to be reseted.
    connect(m_scene, &SofaScene::rootNodeChanged, this, &SofaSceneItemModel::handleRootNodeChange);
    emit sofaSceneChanged();
    emit handleRootNodeChange();
}

void SofaSceneItemModel::handleRootNodeChange()
{    
    /// At this step the m_root member is still containing the old
    /// scene that will be removed.
    if(m_root.get()!=nullptr)
    {
        m_root->removeListener(this);
    }

    /// Flip the old pointer.
    m_root = m_scene->sofaRootNode();

    /// Register now the listener to the new scene graph.
    m_root->addListener(this);

    beginResetModel();
    endResetModel();
    emit modelHasReset();
//    m_needsRefresh = true;

}


} /// namespace _sofasceneitemmodel_
} /// namespace qtquick
} /// namespace sofa
