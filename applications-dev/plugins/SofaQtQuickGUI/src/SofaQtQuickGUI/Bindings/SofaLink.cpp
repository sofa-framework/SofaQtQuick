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

bool SofaLink::setValueString(const QString &path)
{
    return m_self->read("@"+path.toStdString()) ;
}

QString SofaLink::getValueString()
{
    return QString::fromStdString(m_self->getLinkedPath());
}


}
