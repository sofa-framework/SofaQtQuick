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
#include <QInputDialog>
#include <SofaPython3/Prefab.h>


namespace sofaqtquick::bindings::_sofanode_
{

SofaNode* wrap(DAGNode::SPtr n)
{
    if(n.get()==nullptr)
        return nullptr;
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

sofa::qtquick::SofaComponent* SofaNode::toSofaComponent(sofaqtquick::SofaBaseScene* scene)
{
    msg_deprecated("SofaNode") << "DO NOT USE unless communicating with legacy code";
    return new sofa::qtquick::SofaComponent(scene, m_self.get());
}

SofaNode* SofaNode::addChild(QString name)
{
    if (isPrefab() && !attemptToBreakPrefab())
        return nullptr;

    return wrap(sofa::core::objectmodel::New<DAGNode>(name.toStdString(), self()));
}

bool SofaNode::addChild(BaseNode* node)
{
    if (isInAPrefab() && !attemptToBreakPrefab())
        return false;

    if (self()->getChild(node->getName()) != nullptr)
    {
        bool ok;
        const QString name = QInputDialog::getText(nullptr,
                                                      tr("A sibling with the same name exists"),
                                                      tr("Please rename: "),
                                                      QLineEdit::Normal,
                                                      "New Name", &ok);
        if (ok && name.toStdString() != node->getName())
            node->setName(name.toStdString());
        else
            return false;
    }
    self()->addChild(node);
    return true;
}

SofaNode* SofaNode::getFirstParent() const
{
    return wrap(self()->getFirstParent());
}

SofaNode* SofaNode::getChild(QString name) const
{
    return wrap(self()->getChild(name.toStdString()));
}

SofaNode* SofaNode::getNodeInGraph(QString name) const
{
    return wrap(self()->getNodeInGraph(name.toStdString()));
}

SofaNode* SofaNode::getRoot() const
{
    return wrap(self()->getRoot());
}

SofaBaseObject* SofaNode::addObject(const QString& type, const QVariantMap& arguments)
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

SofaNodeList* SofaNode::children() const
{
    SofaNodeList *list = new SofaNodeList();

    for(auto& child : self()->getChildren())
    {
        list->addSofaNode(child);
    }

    return list;
}


SofaNodeList* SofaNode::parents() const
{
    SofaNodeList *list = new SofaNodeList();

    for(auto& parent : self()->getParents())
    {
        list->addSofaNode(parent);
    }

    return list;
}

SofaBaseObjectList* SofaNode::objects() const
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

using sofa::core::objectmodel::Base;
Base* _getMessageInChild(BaseNode* node)
{
    for(auto& child : node->getChildren())
    {
        Base* base = _getMessageInChild(child);
        if(base)
            return base;
    }

    DAGNode* nnode = dynamic_cast<DAGNode*>(node);
    assert(nnode && "Invalid code...only dagnode are allowed");

    for(auto& object : nnode->getNodeObjects())
    {
        if(object->countLoggedMessages() != 0)
            return object;
    }

    return nullptr;
}

bool SofaNode::hasMessageInChild() const
{
    return _getMessageInChild(rawBase()->toBaseNode()) != nullptr;
}

SofaBase* SofaNode::getFirstChildWithMessage() const
{
    return new SofaBase(_getMessageInChild(rawBase()->toBaseNode()));
}

bool SofaNode::attemptToBreakPrefab()
{
    BaseNode* prefab = getPrefabAncestor(rawBase()->toBaseNode());
    QString title("Warning: This action will break the prefab ");
    title += prefab->getName().c_str();
    if (QMessageBox::question(nullptr, title, tr("Are your sure that you want to proceed?")) == QMessageBox::StandardButton::Yes)
    {
        if (prefab->findData("modulename") != nullptr)
            prefab->removeData(prefab->findData("modulename"));
        if (prefab->findData("prefabname") != nullptr)
            prefab->removeData(prefab->findData("prefabname"));
        if (prefab->findData("modulepath") != nullptr)
            prefab->removeData(prefab->findData("modulepath"));
        if (prefab->findData("lineno") != nullptr)
            prefab->removeData(prefab->findData("lineno"));
        if (prefab->findData("sourcecode") != nullptr)
            prefab->removeData(prefab->findData("sourcecode"));
        if (prefab->findData("docstring") != nullptr)
            prefab->removeData(prefab->findData("docstring"));
        if (prefab->findData("args") != nullptr)
            prefab->removeData(prefab->findData("args"));
        dynamic_cast<sofapython3::Prefab*>(prefab)->breakPrefab();
        return true;
    }
    return false;
}


bool SofaNode::addChild(SofaNode* child)
{
    if(child==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot add a null SofaNode");
        return false;
    }
    return this->addChild(child->self());
}

bool SofaNode::addObject(SofaBaseObject* obj)
{
    if (isInAPrefab() && !attemptToBreakPrefab())
        return false;
    if(obj==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "Cannot add a null SofaBaseObject.");
        return false;
    }

    if (dynamic_cast<sofa::simulation::Node*>(self())->getObject(obj->getName().toStdString()) != nullptr)
    {
        bool ok;
        const QString name = QInputDialog::getText(nullptr,
                                                      tr("A sibling with the same name exists"),
                                                      tr("Please rename: "),
                                                      QLineEdit::Normal,
                                                      "New Name", &ok);
        if (ok && name != obj->getName())
            obj->setName(name);
        else
            return false;
    }

    self()->addObject(obj->selfptr());
    return true;
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
SofaNode* SofaNode::removeChildByName(const QString& name)
{
    SofaNode* n = getChild(name);
    removeChild(n);
    return n;
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

QString SofaNode::getNextName(const QString& name) const
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

QString SofaNode::getNextObjectName(const QString& name) const
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

bool SofaNode::rename(const QString& name)
{
    for(auto p : dynamic_cast<sofa::core::objectmodel::BaseNode*>(m_self.get())->getParents())
    {
        auto obj = dynamic_cast<sofa::simulation::Node*>(p)->getObject(name.toStdString());
        if (obj != nullptr && obj->getPathName() != m_self->getPathName())
            return false;
        for (auto c : p->getChildren())
            if (c->getName() == name.toStdString() && c->getPathName() != m_self->getPathName())
                return false;
    }
    m_self->setName(name.toStdString());
    return true;
}


QObject* getItem(SofaBaseObject* self, const QString& name)
{
    if (name == "")
        return self;

    QObject* data = self->findData(name);
    if (data)
        return data;
    SofaLink* link = self->findLink(name);
    if (link)
        return link;
    msg_error("Invalid Syntax");
    return nullptr;
}

QObject* getItem(SofaNode* self, QStringList& path)
{
    if (path.empty())
        return self;

    if (path.size() > 2)
    {
        SofaNode* child = self->getChild(path.front());
        path.pop_front();
        return getItem(child, path);
    }
    if (path.empty())
        return self;
    SofaNode* child = self->getChild(path.front());
    SofaBaseObject* obj = self->getObject(path.front());
    QObject* data = self->findData(path.front());
    if (child)
    {
        path.pop_front();
        return getItem(child, path);
    }
    if (obj)
    {
        path.pop_front();
        if (path.size() > 1)
        {
            msg_error("Invalid Syntax");
            return nullptr;
        }
        if (path.isEmpty())
            return obj;
        return getItem(obj, path.last());
    }
    if (data)
        return data;
    return nullptr; // should never get there
}


QObject* SofaNode::at(const QString& p) const
{
    QString path = p;
    // Invalid path
    if (path == "")
    {
        msg_error("Invalid path provided");
        return nullptr;
    }

    // for absolute paths, resolve relatively to root
    if (path.startsWith("/"))
    {
        path.remove(0,1);
        return this->getRoot()->at(path);
    }
    QStringList stringlist = path.split(".");
    if (stringlist.empty() || stringlist.size() > 2)
    {
        msg_error("Invalid path provided");
        return nullptr;
    }
    // relative paths with a "."
    if (stringlist.size() == 2)
    {
        if (stringlist.first() == "")
        {
            // special case node[".attr1"]
            return getData(stringlist.last());
        }

        QString s = stringlist.last();
        stringlist = stringlist.first().split("/");
        stringlist.push_back(s);
    }
    else
    {
        stringlist = stringlist.first().split("/");
    }

    // Guarantee there's no empty link in the path chain
    for (const auto& string : stringlist)
        if (string == "")
        {
            msg_error("Invalid path provided");
            return nullptr;
        }
    // finally search for the object at the given path:
    return getItem(new SofaNode(self()), stringlist);
}

QObject* SofaNode::get(const QString& name) const
{
    if (name == "")
        return nullptr;
    SofaNode* child = getChild(name);
    SofaBaseObject* obj = getObject(name);
    QObject* data = findData(name);
    if (child)
        return child;
    if (obj)
        return obj;
    if (data)
        return data;
    return nullptr; // should never get there
}

}  // namespace sofaqtquick::bindings::_sofanode_
