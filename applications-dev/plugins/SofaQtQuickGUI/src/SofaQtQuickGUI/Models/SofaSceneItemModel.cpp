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

namespace sofa
{
namespace qtquick
{
namespace _sofasceneitemmodel_
{
using sofa::core::objectmodel::Base;


class LambdaVisitor : public simulation::Visitor
{
    std::function<void(sofa::core::objectmodel::BaseNode*)> m_nodeAction;

public:
    LambdaVisitor(std::function<void(sofa::core::objectmodel::BaseNode*)> nodeAction) :
        Visitor(core::ExecParams::defaultInstance())
    {
        m_nodeAction=nodeAction;
    }

    simulation::Visitor::Result processNodeTopDown(simulation::Node* node)
    {
        m_nodeAction(node);
        return RESULT_CONTINUE;
    }
};

SofaSceneItemModel::SofaSceneItemModel(QObject* parent) : QAbstractItemModel(parent), MutationListener()
{

}

SofaSceneItemModel::~SofaSceneItemModel()
{
    if(m_scene){
        LambdaVisitor lambda([this](sofa::core::objectmodel::BaseNode* basenode)
        {
            /// The cast is ok as long as the only kind of node we are manipulating are inherited from
            /// Node (as are the DAGNode).
            Node* node = static_cast<Node*>(basenode);
            node->removeListener(this);
            msg_info("Listener") << "Remove listener: " << node->getName() ;
        });
        m_scene->sofaRootNode()->execute(lambda);
    }

}

QModelIndex SofaSceneItemModel::index(int row, int column, const QModelIndex &parent) const
{
    //qDebug() << "index query " << parent << " : " << row << ", " << column ;

    sofa::core::objectmodel::Base* currentBase ;
    if (!parent.isValid()){
        currentBase=m_scene->sofaRootNode().get();
    }else{
        currentBase=static_cast<sofa::core::objectmodel::Base*>(parent.internalPointer());
    }

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
        if((unsigned int)row < currentNode->object.size())
        {
            b=static_cast<sofa::core::objectmodel::Base*>(currentNode->object[row].get());
            newIndex=createIndex(row, 0, b);
            //qDebug() << "index query " << parent << " : " << row << ", " << column << " node: "
            //         << QString::fromStdString(currentNode->getName()) << " => object " << newIndex << "  " << currentNode->object[row].get()->toBaseNode() ;
        }
        else
        {
            /// Lets find our parent location.
            /// We change the row id so it fit inside the child array.
            int nrow = row - currentNode->object.size();
            if(nrow>=currentNode->child.size()){
                qWarning() << "Invalid row number" ;
                return QModelIndex();
            }
            auto childNode = currentNode->child[nrow].get();
            b=childNode;
            int c = (childNode->getFirstParent() != currentNode) ;
            newIndex=createIndex(row, c, b);
            //qDebug() << "index query " << parent << " : " << row << ", " << column << " node: "
            //         << QString::fromStdString(currentNode->getName()) << " => node " << newIndex  << "  " ;
        }
        //qDebug() << "index for "<< QString::fromStdString(b->getName()) << " is " << newIndex ;

        return newIndex;
    }

    //qDebug() << "index query " << parent << " : " << row << ", " << column << " is invalid" ;

    return QModelIndex(); //createIndex(row, column, nullptr);
}

QModelIndex SofaSceneItemModel::index(Node* node) const
{
    /// By convention any invalid query returns QModelIndex()
    if(node==nullptr)
        return QModelIndex();

    /// In case the node is the root node we return QModelIndex()
    if(node->getFirstParent()==nullptr)
        return QModelIndex();

    Node* parentNode=static_cast<Node*>(node->getFirstParent());
    int d = std::distance(parentNode->child.begin(),
                          std::find(parentNode->child.begin(), parentNode->child.end(), node) ) ;
    return createIndex(parentNode->object.size()+d, 0, static_cast<sofa::core::objectmodel::Base*>(node));
}

QModelIndex SofaSceneItemModel::parent(const QModelIndex &index) const
{
    //qDebug() << "parent query (" << index << ")";
    if(!index.isValid())
        return QModelIndex();

    sofa::core::objectmodel::Base* currentBase = static_cast<sofa::core::objectmodel::Base*>(index.internalPointer()) ;
    if(currentBase->toBaseNode())
    {
        Node* currentNode = static_cast<Node*>(currentBase->toBaseNode());
        if(currentNode->getNbParents()==0)
            return QModelIndex();

        //qDebug() << "   a: " << QString::fromStdString(currentNode->getName());
        Node* parentNode=static_cast<Node*>(currentNode->getFirstParent());
        Node* grandparentNode = static_cast<Node*>(parentNode->getFirstParent());

        int d;
        QModelIndex idx;
        if(grandparentNode==nullptr)
        {
            return QModelIndex();
        }else{
            d = std::distance(grandparentNode->child.begin(),
                              std::find(grandparentNode->child.begin(), grandparentNode->child.end(), parentNode) ) ;
            idx=createIndex(grandparentNode->object.size()+d, 0, static_cast<sofa::core::objectmodel::Base*>(parentNode));
        }
        //qDebug() << "   b: " << QString::fromStdString(parentNode->getName()) << " => " << idx;

        return idx;
    }
    return QModelIndex();
}

int SofaSceneItemModel::rowCount(const QModelIndex &index) const
{   
    //qDebug() << "rowCount " << index ;

    sofa::core::objectmodel::Base* currentBase ;
    if (!index.isValid()){
        currentBase=m_scene->sofaRootNode().get();
    }else{
        currentBase=static_cast<sofa::core::objectmodel::Base*>(index.internalPointer());
        if(index.column()!=0)
            return 0;
    }

    if(currentBase->toBaseNode())
    {
        Node* currentNode = static_cast<Node*>(currentBase->toBaseNode());
        int numRows = currentNode->child.size() + currentNode->object.size();

//        qDebug() << "     " << index << " node: " << QString::fromStdString(currentNode->getName())
//                    << "  => " << numRows;

        return numRows;
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

    sofa::core::objectmodel::Base* currentBase ;
    if (!index.isValid()){
        currentBase=m_scene->sofaRootNode().get();
    }else{
        currentBase=static_cast<sofa::core::objectmodel::Base*>(index.internalPointer());
    }

    /// Test if we are a not and not a baseobject.
    sofa::core::objectmodel::BaseNode* basenode = currentBase->toBaseNode();
    if(!basenode)
        return false;

    Node* node = static_cast<Node*>(basenode);
    return !node->child.empty() || !node->object.empty();
}

int SofaSceneItemModel::columnCount(const QModelIndex &index) const
{
    SOFA_UNUSED(index);
    return 1;
}

QVariant SofaSceneItemModel::data(const QModelIndex &index, int role) const
{    
    //qDebug() << "data: " << index << "role " << role ;
    sofa::core::objectmodel::Base* currentBase;
    sofa::core::objectmodel::BaseNode* currentNode;
    sofa::core::objectmodel::BaseObject* currentObject;

    if (!index.isValid()){
        currentBase=m_scene->sofaRootNode().get();
    }else{
        currentBase=static_cast<sofa::core::objectmodel::Base*>(index.internalPointer());
    }

    currentNode = currentBase->toBaseNode();
    currentObject = currentBase->toBaseObject();

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
            { (int)Roles::Name, "name" },
            { (int)Roles::TypeName, "typename" },
            { (int)Roles::IsNode, "isNode" },
            { (int)Roles::IsMultiParent, "isMultiParent" },
            { (int)Roles::HasMultiParent, "hasMultiParent" },
            { (int)Roles::IsEnabled, "isEnabled" },
            { (int)Roles::Row, "row" },
                                           }};
    return mapping;
}

size_t rrowCount(Node* parent)
{
    return parent->child.size() + parent->object.size() ;
}

void SofaSceneItemModel::addChild(Node* target, Node* child)
{
    msg_info("b") << "=========== Adding a child node to: " << target->getName();

    /// Who is changing
    QModelIndex parentIndex = index(target) ;
    int i = rrowCount(target);

//    qDebug() << "  target is: " << parentIndex;
//    qDebug() << "       child location is: " << i ;

    beginInsertRows(parentIndex, i, i);
    MutationListener::addChild(target,child);
}

void SofaSceneItemModel::addChildDone(Node* target, Node* child)
{
    endInsertRows();
    //msg_info("b") << "========== Adding a child done: " << child->getName();
}


SofaScene* SofaSceneItemModel::sofaScene() const
{
    return m_scene;
}

void SofaSceneItemModel::setSofaScene(SofaScene* newScene)
{
    std::cout << "setSofaScene: " << std::endl;
    if(m_scene){
        LambdaVisitor lambda([this](sofa::core::objectmodel::BaseNode* basenode)
        {
            /// The cast is ok as long as the only kind of node we are manipulating are inherited from
            /// Node (as are the DAGNode).
            Node* node = static_cast<Node*>(basenode);
            node->removeListener(this);
            msg_info("Listener") << "Remove listener: " << node->getName() ;
        });
        m_scene->sofaRootNode()->execute(lambda);
    }

    m_scene = newScene;


    LambdaVisitor lambda([this](sofa::core::objectmodel::BaseNode* basenode)
    {
        /// The cast is ok as long as the only kind of node we are manipulating are inherited from
        /// Node (as are the DAGNode).
        Node* node = static_cast<Node*>(basenode);
        node->addListener(this);
        msg_info("Listener") << "Adding listener: " << node->getName() ;
    });
    m_scene->sofaRootNode()->execute(lambda);
}

} /// namespace _sofasceneitemmodel_
} /// namespace qtquick
} /// namespace sofa
