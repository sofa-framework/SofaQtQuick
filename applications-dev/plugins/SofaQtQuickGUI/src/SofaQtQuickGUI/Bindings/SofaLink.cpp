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

#include <SofaQtQuickGUI/Bindings/SofaLink.h>
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
#include <SofaQtQuickGUI/SofaScene.h>

#include <QString>

#include <sofa/core/objectmodel/BaseLink.h>
using sofa::core::objectmodel::BaseLink;

#include "../DataHelper.h"

namespace sofaqtquick::bindings::_sofalink_
{

SofaLink::SofaLink(BaseLink* self)
{
    m_self = self;
}
/*
QVariant SofaLink::value() const
{
    return sofaqtquick::helper::createQVariantFromData(m_self);
}

bool SofaLink::setValue(const QVariant& value)
{
    if(sofaqtquick::helper::setDataValueFromQVariant(m_self, value))
    {
        emit valueChanged(value);
        return true;
    }
    return false;
}*/

}


namespace sofa
{

namespace qtquick
{

namespace _sofalink_h_
{

using namespace sofa::core::objectmodel;

SofaLink::SofaLink(const SofaComponent* sofaComponent, BaseLink* link) : QObject(),
    m_component(new SofaComponent(*sofaComponent)),
    m_link(link)
{
    m_component->setParent(this);
}

SofaLink::SofaLink(SofaScene* sofaScene, const sofa::core::objectmodel::Base* base, BaseLink* link) : QObject(),
    m_component(new SofaComponent(sofaScene, base)),
    m_link(link)
{
    m_component->setParent(this);
}

SofaLink::SofaLink(const SofaLink& SofaLink) : QObject(),
    m_component(new SofaComponent(*SofaLink.m_component)),
    m_link(SofaLink.m_link)
{
    m_component->setParent(this);
}

SofaComponent* SofaLink::sofaComponent() const
{
    return m_component;
}

bool SofaLink::setValue(const QString &path)
{
    return m_link->read("@"+path.toStdString()) ;
}

} /// _sofalink_h_
} /// qtquick
} /// sofa
