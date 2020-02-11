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
#include <sofa/helper/logging/Message.h>
using sofa::helper::logging::Message;

#include <sofa/core/objectmodel/BaseNode.h>
using sofa::core::objectmodel::BaseNode;

#include <SofaQtQuickGUI/Bindings/SofaBase.h>
using sofaqtquick::bindings::SofaBase;

#include <SofaQtQuickGUI/Bindings/SofaData.h>
using sofaqtquick::bindings::SofaData;

#include <SofaQtQuickGUI/Bindings/SofaLink.h>
using sofaqtquick::bindings::SofaLink;

#include <SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h>
using sofaqtquick::bindings::SofaCoreBindingContext;

#include <sofa/simulation/Node.h>
#include <SofaSimulationGraph/SimpleApi.h>

#include <sofa/core/objectmodel/DataFileName.h>
using sofa::core::objectmodel::DataFileName;
#include <SofaQtQuickGUI/Bindings/SofaDataFileName.h>
using sofaqtquick::bindings::SofaDataFileName;

#include <QQmlContext>

namespace sofaqtquick::bindings::_sofabase_
{

SofaBase::SofaBase()
{
}

SofaBase::SofaBase(Base::SPtr self)
{
    m_self = self;
}

SofaBase::SofaBase(const SofaBase& o)
{
    m_self = o.rawBase();
}

QString SofaBase::getName() const
{
    return QString::fromStdString(m_self->getName());
}

void SofaBase::setName(const QString& name)
{
    m_self->setName(name.toStdString());
}

QString SofaBase::getPathName() const
{
    if(m_self->toBaseNode())
        return QString::fromStdString(m_self->toBaseNode()->getPathName());

    if(m_self->toBaseObject())
        return QString::fromStdString(m_self->toBaseObject()->getPathName());

    return "/";
}

QString SofaBase::getClassName() const
{
    return QString::fromStdString(m_self->getClassName());
}

QStringList SofaBase::getInheritedClassNames() const
{
    QStringList list;
    for (auto& __class : m_self->getClass()->parents)
        list.push_back(__class->className.c_str());
    return list;
}

QString SofaBase::getTemplateName() const
{
    return QString::fromStdString(m_self->getTemplateName());
}

SofaData* SofaBase::findData(const QString& name) const
{
    auto* data = m_self->findData(name.toStdString());
    if(!data)
    {
        return nullptr;
    }
    return new SofaData(data);
}

SofaData* SofaBase::getData(const QString& name) const
{
    auto* data = m_self->findData(name.toStdString());
    if(!data)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "There is no data with name '"+name+" in " + m_self->getName().c_str() + "'");
        return nullptr;
    }
    if (dynamic_cast<DataFileName*>(data))
        return new SofaDataFileName(dynamic_cast<DataFileName*>(data));
    return new SofaData(data);
}

QObject* SofaBase::get(const QString& s)
{
    if (SofaData* d = static_cast<SofaData*>(findData(s)))
    {
        return d;
    }
    if (SofaLink* l = static_cast<SofaLink*>(findLink(s)))
    {
        return l;
    }
    return nullptr;
}



SofaLink* SofaBase::findLink(const QString& name) const
{
    auto* link = m_self->findLink(name.toStdString());
    if(!link)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, "There is no link with name '"+name+"'");
        return nullptr;
    }
    return new SofaLink(link);
}

QStringList SofaBase::getLinks() const
{
    const auto& links = m_self->getLinks();
    QStringList list;
    for (auto& link : links)
        list.append(QString(link->getName().c_str()));
    return list;
}


QStringList SofaBase::getDataFields() const
{
    const sofa::helper::vector<BaseData*>& datafields = m_self->getDataFields();
    QStringList list;
    for (auto& field : datafields)
        list.append(QString(field->getName().c_str()));
    return list;
}

bool SofaBase::hasLocations() const
{
    const Base* base = m_self.get();
    if(base)
    {
        return !base->getDefinitionSourceFileName().empty()
                || !base->getInstanciationSourceFileName().empty() ;
    }
    return false;
}

QString SofaBase::getSourceLocation() const
{
    const Base* base = m_self.get();
    if(base && !(base->getDefinitionSourceFileName().empty()))
    {
        return QString("('%1',%2)")
                .arg(QString::fromStdString(base->getDefinitionSourceFileName()))
                .arg(base->getDefinitionSourceFilePos());
    }
    return "";
}

QString SofaBase::getInstanciationLocation() const
{
    const Base* base = m_self.get();
    if(base && !(base->getInstanciationSourceFileName().empty()))
    {
        return QString("('%1',%2)")
                .arg(QString::fromStdString(base->getInstanciationSourceFileName()))
                .arg(base->getInstanciationSourceFilePos());
    }
    return "";
}

QString SofaBase::output() const
{
    const Base* base = m_self.get();
    if(base)
        return QString::fromStdString(base->getLoggedMessagesAsString({Message::Info, Message::Advice}));

    return QString();
}

bool SofaBase::hasMessage() const
{
    const Base* base = m_self.get();
    if(!base)
        return false;

    return base->countLoggedMessages() != 0;
}

QString SofaBase::warning() const
{
    const Base* base = m_self.get();
    if(base)
        return QString::fromStdString(base->getLoggedMessagesAsString({Message::Deprecated,
                                                                       Message::Warning,
                                                                       Message::Error,
                                                                       Message::Fatal
                                                                      }));

    return QString();
}

void SofaBase::clearOutput() const
{
    Base* base = m_self.get();
    if(base)
    {
        base->clearLoggedMessages();
    }
}

void SofaBase::clearWarning() const
{
    Base* base = m_self.get();
    if(base)
    {
        base->clearLoggedMessages();
    }
}


bool SofaBase::isNode() const
{
    return rawBase()->toBaseNode() != nullptr;
}

bool SofaBase::isSame(SofaBase* sofaComponent) const
{
    if(!sofaComponent)
        return false;

    // same wrapper => same component
    if(this == sofaComponent)
        return true;

    // same base object => same component
    if(rawBase() == sofaComponent->rawBase())
        return true;

    return false;
}

void SofaBase::dump() const
{
    if(isNode())
    {
        Base* base = m_self.get();
        sofa::simpleapi::dumpScene(dynamic_cast<sofa::simulation::Node*>(base->toBaseNode()));
    }
}

QString SofaBase::componentStateAsString() const
{
    std::stringstream s;
    s << rawBase()->d_componentstate.getValue();
    return QString::fromStdString(s.str());
}


} /// sofaqtquick
