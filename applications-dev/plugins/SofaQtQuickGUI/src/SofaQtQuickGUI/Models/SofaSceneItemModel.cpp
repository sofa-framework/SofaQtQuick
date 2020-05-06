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
#include <QDebug>
#include <sofa/simulation/Visitor.h>
#include "SofaSceneItemModel.h"
#include "SofaSimulationGraph/DAGNode.h"
#include <SofaQtQuickGUI/Bindings/SofaCoreBindingFactory.h>
using sofaqtquick::bindings::SofaCoreBindingFactory;

namespace sofaqtquick
{
namespace _sofasceneitemmodel_
{
using sofa::core::objectmodel::Base;
using sofa::core::objectmodel::BaseObject;
using sofa::core::objectmodel::BaseNode;
using sofa::simulation::Node;


SofaSceneItemModel::SofaSceneItemModel(QObject* parent) : QAbstractItemModel(parent), MutationListener()
{
    connect(&m_refreshModel, &QTimer::timeout, this, &SofaSceneItemModel::onTimeOutModelRefresh);
    m_refreshModel.start(200);

}

SofaSceneItemModel::~SofaSceneItemModel()
{
    if(m_root!=nullptr)
        m_root->removeListener(this);
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

void SofaSceneItemModel::emitAllChanges(Node* node)
{
    if (node == nullptr)
        return;

    for (BaseObject::SPtr object: node->object)
    {
        if( m_dataTracker.hasChanged(object->m_componentstate) )
        {
            QModelIndex t = index(node, object.get());
            emit dataChanged(t,t);
        }
    }

    for (Node::SPtr child: node->child)
    {
        emitAllChanges(child.get());
    }

    if( m_dataTracker.hasChanged(node->m_componentstate) )
    {
        QModelIndex t = index(node);
        emit dataChanged(t,t);
    }
}

void SofaSceneItemModel::onTimeOutModelRefresh()
{
    /// Recursively emit the dataChanged signal on object that have changed.
    emitAllChanges(m_root.get());
    m_dataTracker.clean();
}

QModelIndex SofaSceneItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!m_scene) return QModelIndex();
    Base* currentBase ;
    if (!parent.isValid())
    {
        currentBase=m_scene->sofaRootNode().get();
        if (!row && !column)
        {
            return createIndex(0, 0, static_cast<Base*>(currentBase->toBaseNode()));
        }
        else
        {
            qDebug() << "index query " << parent << " : " << row << ", " << column << " is invalid" ;
            return QModelIndex();
        }
    }
    currentBase=static_cast<Base*>(parent.internalPointer());

    if(currentBase->toBaseNode())
    {
        QModelIndex newIndex ;
        Node* currentNode = dynamic_cast<Node*>(currentBase);

        /// The row number is encoding either a node or a component.
        /// Its values are between 0 and object.size()+childNode.size().
        /// The components are before the child node so detect if the row is refereing to a
        /// component index or a child node index we are checking if it larger than the components's
        /// container size().
        Base* b;
        if(size_t(row) < currentNode->object.size())
        {
            b=static_cast<Base*>(currentNode->object[unsigned(row)].get());
            newIndex=createIndex(row, 0, b);
        }
        else
        {
            /// Lets find our parent location.
            /// We change the row id so it fit inside the child array.
            size_t nrow = size_t(row) - currentNode->object.size();
            if(nrow >= currentNode->child.size())
            {
                return QModelIndex();
            }
            auto childNode = currentNode->child[unsigned(nrow)].get();
            b=childNode;
            int c = (childNode->getNbParents() != 1) ;
            newIndex=createIndex(row, c, b);
        }

        return newIndex;
    }

    return QModelIndex();
}

QModelIndex SofaSceneItemModel::index(Node* node) const
{
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
    return createIndex(int(d), c, static_cast<Base*>(node));
}

QModelIndex SofaSceneItemModel::index(Node* parent, BaseObject* obj) const
{

    if(obj == nullptr || parent == nullptr){
        qDebug() << "Object index query parent/obj (" << (long)parent << ", " << (long)obj <<  ")";

        return QModelIndex();
    }

    size_t d = size_t(std::distance(parent->object.begin(),
                                    std::find(parent->object.begin(), parent->object.end(), obj))) ;
    return createIndex(int(d), 0, static_cast<Base*>(obj));
}

QModelIndex SofaSceneItemModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    Base* currentBase = static_cast<Base*>(index.internalPointer()) ;
    if (!currentBase)
    {
        qDebug() << "SofaSceneItemModel::parent: missing currentBase!" << index;
        return QModelIndex();
    }

    Node* parentNode=nullptr;
    if(currentBase->toBaseNode())                   ///< Handle the case where the data is a node.
    {
        Node* currentNode = static_cast<Node*>(currentBase->toBaseNode());
        if (!currentNode || !currentNode->getFirstParent())
        {
            return QModelIndex();
        }
        parentNode =  static_cast<Node*>(currentNode->getFirstParent());
    }else
    {
        parentNode = static_cast<Node*>(currentBase->toBaseObject()->getContext());
    }

    if(!parentNode)
    {
        qDebug() << "SofaSceneItemModel::parent: missing parent Node!" << index;
        return QModelIndex();
    }
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
        idx=createIndex(int(grandparentNode->object.size()+d), c, static_cast<Base*>(parentNode));
    }

    return idx;
}

int SofaSceneItemModel::rowCount(const QModelIndex &index) const
{   
    if (!index.isValid())
        return 1;
    if(index.column()>0)
        return 0;
    Base* currentBase=static_cast<Base*>(index.internalPointer());

    if(currentBase->toBaseNode())
    {
        Node* currentNode = static_cast<Node*>(currentBase->toBaseNode());
        size_t numRows = currentNode->child.size() + currentNode->object.size();
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
}

int SofaSceneItemModel::columnCount(const QModelIndex &index) const
{
    SOFA_UNUSED(index);
    return 1;
}


sofaqtquick::bindings::SofaBase* SofaSceneItemModel::getBaseFromIndex(const QModelIndex& index) const
{
    if (!index.isValid())
        return nullptr;

    Base* currentBase=static_cast<Base*>(index.internalPointer());

    if(!currentBase)
        return nullptr;

    return SofaCoreBindingFactory::wrap(currentBase);
}

QModelIndex SofaSceneItemModel::getIndexFromBase(sofaqtquick::bindings::SofaBase* component) const
{
    if (component)
    {
        Node* node=dynamic_cast<Node*>(component->rawBase());
        if(node)
            return index(node);

        BaseObject* obj = dynamic_cast<BaseObject*>(component->rawBase());
        if(obj)
            return index(static_cast<Node*>(obj->getContext()->toBaseNode()), obj);
    }
    return QModelIndex();
}


QVariant SofaSceneItemModel::data(const QModelIndex &index, int role) const
{    
    Base* currentBase;
    BaseNode* currentNode;

    if (!index.isValid()){
        return false;
    }else{
        currentBase=static_cast<Base*>(index.internalPointer());
    }

    currentNode = currentBase->toBaseNode();

    switch(static_cast<Roles>(role))
    {
    case Roles::Name:
        return QVariant(QString::fromStdString(currentBase->getName()));
    case Roles::TypeName:
        return QVariant::fromValue(QString::fromStdString(currentBase->getClassName()));
    case Roles::ShortName:
        return QVariant::fromValue(QString::fromStdString(currentBase->getClass()->shortName));
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
    case Roles::Status:
        return QVariant(QString::fromStdString(currentBase->d_componentstate.getValueString()));
    }

    return QVariant(QString("INVALID ROLE"));
}

QHash<int, QByteArray> SofaSceneItemModel::roleNames() const
{
    /// Create a static mapping to link the "role" number as Qt is using and the corresponding
    /// textual represresentation; The Roles are encoded using an 'enum class' so they don't
    /// automatically convert to int value in the constructor initializer_list.
    static QHash<int, QByteArray> mapping {{
            { int(Roles::Name), "name" },
            { int(Roles::TypeName), "typename" },
            { int(Roles::ShortName), "shortname" },
            { int(Roles::IsNode), "isNode" },
            { int(Roles::IsMultiParent), "isMultiParent" },
            { int(Roles::HasMultiParent), "hasMultiParent" },
            { int(Roles::IsEnabled), "isEnabled" },
            { int(Roles::Row), "row" },
            { int(Roles::Status), "statusString" }}};
    return mapping;
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
            startTime = testTime;
            if (this->m_needsRefresh)
                emit resetRequired();
        }
    }
}

void SofaSceneItemModel::onResetRequired()
{
    beginResetModel();
    endResetModel();
    emit modelHasReset();
    this->m_needsRefresh = false;
}


void SofaSceneItemModel::onBeginAddChild(Node* target, Node* child)
{
    SOFA_UNUSED(child);
    QModelIndex parentIndex = index(target);
    int i = int(rrowCount(target));
    beginInsertRows(parentIndex, i, i);
}

void SofaSceneItemModel::addNodeContent(Node* node)
{
    QModelIndex idx = index(node);
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

void SofaSceneItemModel::onEndAddChild(Node* target, Node* child)
{
    SOFA_UNUSED(target);
    SOFA_UNUSED(child);

    endInsertRows();

    /// call onAddChildBegin / onAddChildEnd for each sub nodes
    for (Node::SPtr n : child->child)
    {
        addNodeContent(n.get());
    }
}

/// because indexes to be removed must have their actual parent,
/// recursive removal of a node's childs must be done with this function
void SofaSceneItemModel::removeNodeContent(Node* node)
{
    QModelIndex idx = index(node);

    /// Remove the content of each subnode
    for (Node::SPtr child : node->child)
        removeNodeContent(child.get());

    /// Remove 1st level content of the current node
    beginRemoveRows(idx, 0, int(rrowCount(node)));
    endRemoveRows(); // we end the removal immediately
}


void SofaSceneItemModel::onBeginRemoveChild(Node* parent, Node* child)
{
    QModelIndex parentIndex = index(parent);
    int childIndex = index(child).row();

    removeNodeContent(child);
    beginRemoveRows(parentIndex, childIndex, childIndex);
}

void SofaSceneItemModel::onEndRemoveChild(Node* parent, Node* child)
{
    SOFA_UNUSED(child);
    SOFA_UNUSED(parent);
    endRemoveRows();
}

void SofaSceneItemModel::onBeginAddObject(Node* parent, sofa::core::objectmodel::BaseObject* obj)
{
    SOFA_UNUSED(obj);
    QModelIndex parentIndex = index(parent);
    int objIndex = int(parent->object.size());
    beginInsertRows(parentIndex, objIndex, objIndex);
}

void SofaSceneItemModel::onEndAddObject(Node* parent, sofa::core::objectmodel::BaseObject* obj)
{
    SOFA_UNUSED(obj);
    SOFA_UNUSED(parent);
    endInsertRows();
}
void SofaSceneItemModel::onBeginRemoveObject(Node* parent, sofa::core::objectmodel::BaseObject* obj)
{
    QModelIndex parentIndex = index(parent);
    int objIndex = index(parent, obj).row();
    beginRemoveRows(parentIndex, objIndex, objIndex);

}
void SofaSceneItemModel::onEndRemoveObject(Node* parent, sofa::core::objectmodel::BaseObject* obj)
{
    SOFA_UNUSED(obj);
    SOFA_UNUSED(parent);
    endRemoveRows();
}


SofaBaseScene* SofaSceneItemModel::sofaScene() const
{
    return m_scene;
}

void SofaSceneItemModel::setSofaScene(SofaBaseScene* newScene)
{    
    m_scene = newScene;
    emit sofaSceneChanged();
    emit handleRootNodeChange();
    if (!m_scene) return;

    /// The scene passed to this model is tracked to monitor if its status has changed.
    /// If this is the case then the model needs to be reseted.
    connect(m_scene, &SofaBaseScene::rootNodeChanged, this, &SofaSceneItemModel::handleRootNodeChange);
}

void SofaSceneItemModel::handleRootNodeChange()
{    
    /// At this step the m_root member is still containing the old
    /// scene that will be removed.
    if(m_root.get()!=nullptr)
    {
        m_root->removeListener(this);
    }

    if(m_scene==nullptr)
        return ;
    /// Flip the old pointer.
    m_root = m_scene->sofaRootNode();

    if(m_root==nullptr)
        return ;

    /// Register now the listener to the new scene graph.
    m_root->addListener(this);

    beginResetModel();
    endResetModel();
    emit modelHasReset();
}


} /// namespace _sofasceneitemmodel_
} /// namespace sofaqtquick
