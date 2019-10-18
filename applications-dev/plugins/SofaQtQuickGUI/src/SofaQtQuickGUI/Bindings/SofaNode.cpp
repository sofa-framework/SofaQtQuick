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
using sofa::core::objectmodel::BaseObject;
using sofa::core::objectmodel::BaseNode;

#include "SofaComponent.h"

#include <SofaSimulationGraph/SimpleApi.h> ///< To create object in a slow but easy way.

#include <SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h>
using sofaqtquick::bindings::SofaCoreBindingContext;

#include <SofaQtQuickGUI/Bindings/SofaCoreBindingFactory.h>
using sofaqtquick::bindings::SofaCoreBindingFactory;

#include <sofa/core/ExecParams.h>
using sofa::core::ExecParams;

#include <SofaQtQuickGUI/DataHelper.h>
#include <QMessageBox>


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
SofaNode* wrap(BaseNode* n)
{
    return wrap(static_cast<DAGNode*>(n));
}

SofaNode::SofaNode(const QString name, QObject *parent)
{
    SOFA_UNUSED(parent);
    m_self = sofa::core::objectmodel::New<DAGNode>(name.toStdString());
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
    msg_deprecated("SofaNode") << "DO NOT USE unless communicating with legacy code";
    return new sofa::qtquick::SofaComponent(scene, m_self.get());
}

SofaNode* SofaNode::createChild(QString name)
{
    if (isPrefab() && !attemptToBreakPrefab())
        return nullptr;

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

SofaBaseObject* SofaNode::createObject(const QString& type, const QVariantMap& arguments)
{
    if (isPrefab() && !attemptToBreakPrefab())
        return nullptr;

    /// Super slow but easy to write object constructor.
    std::map<std::string, std::string> args;
    for(auto& key : arguments.keys())
    {
        args[key.toStdString()] = arguments[key].toString().toStdString();
    }

    auto o = sofa::simpleapi::createObject(selfptr(), type.toStdString(), args);
    if(o!=nullptr)
        return new SofaBaseObject(o);
    return nullptr;
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

SofaBaseObjectList* SofaNode::getBaseObjects()
{
    SofaBaseObjectList *list = new SofaBaseObjectList();

    for(auto& obj : self()->object)
    {
        list->addSofaBaseObject(obj.get());
    }

    return list;
}

BaseNode* SofaNode::getPrefabAncestor(BaseNode* n) const
{
    if (n)
    {
        if (n->findData("prefabname")) return n;
        for (const auto& p : n->getParents())
        {
            BaseNode* prefab = getPrefabAncestor(p);
            if (prefab)
                return prefab;
        }
    }
    return nullptr;
}


bool SofaNode::isPrefab() const
{
    BaseNode* n = rawBase()->toBaseNode();
    if (n)
    {
        return (n->findData("prefabname"))?true:false;
    }
    return false;
}

bool SofaNode::isInAPrefab() const
{
    BaseNode* n = rawBase()->toBaseNode();
    return getPrefabAncestor(n);
}

bool _hasMessageInChild(BaseNode* node)
{
    for(auto& child : node->getChildren())
    {
        if(_hasMessageInChild(child))
            return true;
    }

    DAGNode* nnode = dynamic_cast<DAGNode*>(node);
    assert(nnode && "Invalid code...only dagnode are allowed");

    for(auto& object : nnode->getNodeObjects())
    {
        if(object->countLoggedMessages() != 0)
            return true;
    }

    return false;
}

bool SofaNode::hasMessageInChild() const
{
    return _hasMessageInChild(rawBase()->toBaseNode());
}

bool SofaNode::attemptToBreakPrefab()
{
    BaseNode* prefab = getPrefabAncestor(rawBase()->toBaseNode());
    QString title("Warning: This action will break the prefab ");
    title += prefab->getName().c_str();
    if (QMessageBox::question(nullptr, title, tr("Are your sure that you want to proceed?")) == QMessageBox::StandardButton::Yes)
    {
        prefab->removeData(prefab->findData("modulename"));
        prefab->removeData(prefab->findData("prefabname"));
        prefab->removeData(prefab->findData("modulepath"));
        prefab->removeData(prefab->findData("lineno"));
        prefab->removeData(prefab->findData("sourcecode"));
        prefab->removeData(prefab->findData("docstring"));
        prefab->removeData(prefab->findData("args"));
        return true;
    }
    return false;
}


void SofaNode::addChild(SofaNode* child)
{
    if (isInAPrefab() && !attemptToBreakPrefab())
        return;
    if(child==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot add a null SofaNode");
        return;
    }
    self()->addChild(child->selfptr());
}

void SofaNode::addObject(SofaBaseObject* obj)
{
    if (isInAPrefab() && !attemptToBreakPrefab())
        return;
    if(obj==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot add a null SofaBaseObject.");
        return;
    }
    self()->addObject(obj->selfptr());
}

void SofaNode::copyTo(SofaNode* node)
{
    if ((isInAPrefab() && !attemptToBreakPrefab()) ||
            (node->isInAPrefab() && !node->attemptToBreakPrefab()))
        return;
    if(node==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot copy node's content to a null SofaNode.");
        return;
    }
    DAGNode* n = self();
    for (auto& child : n->getChildren())
    {
        if (!child || child->getName().empty())
            continue;
        child->setName(node->getNextName(QString(child->getName().c_str())).toStdString());
        node->addChild(new SofaNode(DAGNode::SPtr(dynamic_cast<DAGNode*>(child))));
        n->removeChild(child);
    }
    for (unsigned i = 0 ; i < n->object.size() ; ++i)
    {
        BaseObject::SPtr object = n->object[i];
        if (!object || object->getName().empty())
            continue;
        object->setName(node->getNextObjectName(QString(object->getName().c_str())).toStdString());
        node->addObject(new SofaBaseObject(BaseObject::SPtr(object)));
        n->removeObject(object);
    }
}





void SofaNode::moveChild(SofaNode* node, SofaNode* prev_parent)
{
    if ((isInAPrefab() && !attemptToBreakPrefab())
            || (prev_parent->isInAPrefab() && !prev_parent->attemptToBreakPrefab()))
        return;

    DAGNode* _this = self();
    dynamic_cast<sofa::simulation::Node*>(_this)->moveChild(node->selfptr(), prev_parent->selfptr());
}

void SofaNode::moveObject(SofaBaseObject* obj)
{
    BaseNode* p = obj->selfptr()->getContext()->toBaseNode()->getFirstParent();
    SofaNode prev_parent(DAGNode::SPtr(dynamic_cast<DAGNode*>(p)));
    if ((isInAPrefab() && !attemptToBreakPrefab())
            || (prev_parent.isInAPrefab() && !prev_parent.attemptToBreakPrefab()))
        return;
    DAGNode* _this = self();
    _this->moveObject(obj->selfptr());
}





void SofaNode::removeChild(SofaNode* node)
{
    if (isInAPrefab() && !attemptToBreakPrefab())
        return;

    self()->removeChild(node->self());
}

void SofaNode::removeObject(SofaBaseObject* obj)
{
    if (isInAPrefab() && !attemptToBreakPrefab())
        return;

    self()->removeObject(obj->self());
}





void SofaNodeList::addSofaNode(BaseNode* node)
{
    m_list.push_back(wrap(node));
}

unsigned int SofaNodeList::size()
{
    return unsigned(m_list.size());
}

SofaNode* SofaNodeList::at(unsigned int n)
{
    return m_list.at(n);
}

SofaNode* SofaNodeList::first()
{
    return m_list.front();
}

SofaNode* SofaNodeList::last()
{
    return m_list.back();
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

QString SofaNode::getNextObjectName(const QString& name)
{
    int i = 1;
    QString newname = name;

    while(dynamic_cast<sofa::simulation::Node*>(self())->getObject(newname.toStdString())!=nullptr)
    {
        newname = name + QString::number(i);
        i++;
    }
    return newname;
}

QObject* SofaNode::get(const QString& path) const
{
    /// Searching for a data if there is a "." separator.
    if(path.contains("."))
    {
        /// search for the "name" data of the component (this data is always present if the component exist)
        sofa::core::objectmodel::BaseData* data = sofaqtquick::helper::findData(self(), "@" + path);

        if(!data)
            return nullptr;

        return new SofaData(data);
    }

    /// If this is not a data. We are searching for the data with .name. To retrive the owner.
    sofa::core::objectmodel::BaseData* data = sofaqtquick::helper::findData(self(), "@" + path + ".name");
    if(!data)
        return nullptr;

    sofa::core::objectmodel::Base* base = data->getOwner();
    if(!base)
        return nullptr;

    return SofaCoreBindingFactory::wrap(base);
}

}  // namespace sofaqtquick::bindings::_sofanode_
