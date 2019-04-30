/*
Copyright 2018, CNRS, INRIA

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
*/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/
#include <QString>

#include <SofaQtQuickGUI/Bindings/SofaLink.h>
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
#include <SofaQtQuickGUI/SofaScene.h>

#include <SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h>
using sofaqtquick::bindings::SofaCoreBindingContext;

#include <sofa/core/objectmodel/BaseLink.h>
using sofa::core::objectmodel::BaseLink;

#include "../DataHelper.h"
namespace sofaqtquick::bindings::_sofalink_
{


template<class T, class IN>
T* wrap(IN* ptr, QString msg)
{
    if(ptr==nullptr)
    {
        SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::GenericError, msg);
        return nullptr;
    }
    return new T(ptr);
}

SofaLink::SofaLink(BaseLink* self)
{
    m_self = self;
}

unsigned int SofaLink::getSize()
{
    return m_self->getSize();
}

SofaBase* SofaLink::getLinkedBase(size_t index)
{
    if(index >= m_self->getSize())
    {
        if(index > 0)
            SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::RangeError, "Invalid index.");
        return nullptr;
    }
    return wrap<SofaBase, sofa::core::objectmodel::Base>(m_self->getLinkedBase(index),
                                                         "Unable to get SofaBase.");
}

SofaData* SofaLink::getLinkedData(size_t index)
{
    if(index >= m_self->getSize())
    {
        if(index > 0)
            SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::RangeError, "Invalid index.");
        return nullptr;
    }
    return wrap<SofaData, sofa::core::objectmodel::BaseData>(m_self->getLinkedData(index),
                                                         "Unable to get SofaData.");
}

QString SofaLink::getLinkedPath(size_t index)
{
    if(index >= m_self->getSize())
    {
        if(index > 0)
            SofaCoreBindingContext::getQQmlEngine()->throwError(QJSValue::RangeError, "Invalid index.");
        return nullptr;
    }
    return QString::fromStdString(m_self->getLinkedPath(index));
}


}
