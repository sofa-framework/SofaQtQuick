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

#include <SofaQtQuickGUI/Bindings/SofaBase.h>

#include <sofa/core/objectmodel/BaseNode.h>
using sofa::core::objectmodel::BaseNode;

#include <SofaQtQuickGUI/Bindings/SofaData.h>
using sofaqtquick::bindings::SofaData;

namespace sofaqtquick::bindings::_sofabase_
{

SofaBase::SofaBase()
{
}

SofaBase::SofaBase(Base::SPtr self)
{
    m_self = self;
}

QString SofaBase::getName() const
{
    return QString::fromStdString(m_self->getName());
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

QString SofaBase::getTemplateName() const
{
    return QString::fromStdString(m_self->getTemplateName());
}

QObject* SofaBase::getData(const QString& name) const
{
    BaseData* data = m_self->findData(name.toStdString());
    if(!data)
        return nullptr;
    return new SofaData(data);
}

bool SofaBase::hasLocations() const
{
    const Base* base = m_self.get();
    if(base)
    {
        return !base->getSourceFileName().empty()
            || !base->getInstanciationFileName().empty() ;
    }
    return false;
}

QString SofaBase::getSourceLocation() const
{
    const Base* base = m_self.get();
    if(base && !(base->getSourceFileName().empty()))
    {
        return QString("('%1',%2)")
                .arg(QString::fromStdString(base->getSourceFileName()))
                .arg(base->getSourceFilePos());
    }
    return "";
}

QString SofaBase::getInstanciationLocation() const
{
    const Base* base = m_self.get();
    if(base && !(base->getInstanciationFileName().empty()))
    {
        return QString("('%1',%2)")
                .arg(QString::fromStdString(base->getInstanciationFileName()))
                .arg(base->getInstanciationFilePos());
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
        base->clearOutputs();
    }
}

void SofaBase::clearWarning() const
{
    Base* base = m_self.get();
    if(base)
    {
        base->clearWarnings();
    }
}


bool SofaBase::isNode() const
{
    return rawBase()->toBaseNode() != nullptr;
}


} /// sofaqtquick
