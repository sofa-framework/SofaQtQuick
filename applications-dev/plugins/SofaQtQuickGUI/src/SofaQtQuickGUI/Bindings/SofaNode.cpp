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

#include <SofaSimulationGraph/SimpleApi.h> ///< To create object in a slow but easy way.

#include <SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h>
using sofaqtquick::bindings::SofaCoreBindingContext;

#include <sofa/core/ExecParams.h>
using sofa::core::ExecParams;


namespace sofaqtquick::bindings::_sofanode_
{

SofaNode* wrap(DAGNode::SPtr n)
{
    if(n.get()==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Unable to get node.");
        return nullptr;
    }
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

SofaNode::SofaNode(SofaBase* self)
{
    if (self->isNode())
        m_self = self->base();
    else {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "This SofaBase is not a node. Cannot Downcast");
    }
}

SofaNode::~SofaNode(){}

SofaNode* SofaNode::createFrom(sofa::core::objectmodel::Base* obj)
{
    return wrap(obj->toBaseNode());
}

void SofaNode::init() const
{
    self()->init(ExecParams::defaultInstance());
}

void SofaNode::reinit() const
{
    self()->reinit(ExecParams::defaultInstance());
}

sofa::qtquick::SofaComponent* SofaNode::toSofaComponent(sofa::qtquick::SofaScene* scene)
{
    msg_warning("SofaNode::toSofaComponent") << "DEPRECATED Method DO NOT USE unless communicating with legacy code";
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

SofaBaseObject* SofaNode::createObject(const QString& type, const QVariantMap& arguments) const
{
    /// Super slow but easy to write object constructor.
    std::map<std::string, std::string> args;
    for(auto& key : arguments.keys())
    {
        args[key.toStdString()] = arguments[key].toString().toStdString();
    }

    auto o = sofa::simpleapi::createObject(selfptr(), type.toStdString(), args);
    return new SofaBaseObject(o);
}


SofaBaseObject* SofaNode::getObject(const QString& path) const
{
    BaseObject::SPtr sptr;
    self()->get<BaseObject>(sptr, path.toStdString());
    if (!sptr)
        return nullptr;
    return new SofaBaseObject(sptr.get());
}

SofaNodeList* SofaNode::getChildren()
{
    SofaNodeList *list = new SofaNodeList();

    for(auto& child : self()->getChildren())
    {
        list->addSofaNode(child);
    }

    return list;
}


void SofaNode::addChild(SofaNode* child)
{
    if(child==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot add a null SofaNode");
        return;
    }
    self()->addChild(child->selfptr());
}

void SofaNode::addObject(SofaBaseObject* obj)
{
    if(obj==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot add a null SofaBaseObject.");
        return;
    }
    self()->addObject(obj->selfptr());
}

void SofaNodeList::addSofaNode(sofa::core::objectmodel::BaseNode* node)
{
    m_list.push_back(wrap(node));
}

unsigned int SofaNodeList::size()
{
    return m_list.size();
}

SofaNodeFactory::SofaNodeFactory(){}
SofaNode* SofaNodeFactory::createInstance(SofaBase* b)
{
    if(b==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError,
                                                            "Cannot get a SofaNode from a nullptr.");
        return nullptr;
    }

    if(!b->isNode())
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError,
                                                            "Argument type is not compatible with SofaNode.");
        return nullptr;
    }

    return wrap(b->base()->toBaseNode());
}

QString SofaNode::getNextName(const QString& name)
{
    int i = 1;
    QString newname = name;
    while(self()->getChild(newname.toStdString())!=nullptr)
    {
        newname = name + QString::number(i);
        i++;
    }
    return newname;
}



}  // namespace sofaqtquick::bindings::_sofanode_
