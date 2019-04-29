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
#include "SofaNode.h"
#include "SofaComponent.h"

#include <SofaSimulationGraph/SimpleApi.h>
#include <SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h>
using sofaqtquick::bindings::SofaCoreBindingContext;

namespace sofaqtquick::bindings::_sofanode_
{

SofaNode* wrap(DAGNode::SPtr n)
{
    if(n.get()==nullptr)
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Unable to get node.");

    return new SofaNode(n);
}
SofaNode* wrap(DAGNode* n)
{
    return wrap(DAGNode::SPtr(n));
}
SofaNode* wrap(sofa::simulation::Node* n)
{
    return wrap(dynamic_cast<DAGNode*>(n));
}
SofaNode* wrap(sofa::core::objectmodel::BaseNode* n)
{
    return wrap(static_cast<DAGNode*>(n));
}

SofaNode::SofaNode(QObject *parent)
{
    SOFA_UNUSED(parent);
    m_self = sofa::core::objectmodel::New<DAGNode>("unnamed");
}

SofaNode::SofaNode(DAGNode::SPtr self, QObject *parent)
{
    SOFA_UNUSED(parent);
    m_self = self;
}

SofaNode::~SofaNode(){}

sofa::qtquick::SofaComponent* SofaNode::toSofaComponent(sofa::qtquick::SofaScene* scene)
{
    return new sofa::qtquick::SofaComponent(scene, m_self.get());
}

SofaNode* SofaNode::createChild(QString name)
{
    return wrap(sofa::core::objectmodel::New<DAGNode>(name.toStdString(), self()));
}

SofaNode* SofaNode::getFirstParent()
{
    return wrap(self()->getFirstParent());
}

SofaNode* SofaNode::getChild(QString name)
{
    return wrap(self()->getChild(name.toStdString()));
}

SofaNode* SofaNode::getNodeInGraph(QString name)
{
    return wrap(self()->getNodeInGraph(name.toStdString()));
}

SofaNode* SofaNode::getRoot()
{
    return wrap(self()->getRoot());
}

void SofaNode::addChild(SofaNode* child)
{
    if(child==nullptr)
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot add a null SofaNode");

    self()->addChild(child->selfptr());
}

void SofaNode::addObject(SofaBaseObject* obj)
{
    if(obj==nullptr)
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot add a null SofaBaseObject.");

    self()->addObject(obj->selfptr());
}

}  // namespace sofaqtquick::bindings::_sofanode_
