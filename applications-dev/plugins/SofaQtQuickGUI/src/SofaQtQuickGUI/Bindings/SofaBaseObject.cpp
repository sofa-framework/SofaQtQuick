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
#include "SofaBaseObject.h"

namespace sofaqtquick::bindings::_sofaobject_
{

SofaBaseObject::SofaBaseObject(QObject *parent)
{
    SOFA_UNUSED(parent);
    m_self = sofa::core::objectmodel::New<BaseObject>("unnamed");
}

SofaBaseObject::SofaBaseObject(BaseObject::SPtr self, QObject *parent)
{
    SOFA_UNUSED(parent);
    m_self = self;
}

SofaBaseObject::~SofaBaseObject(){}

}  // namespace sofaqtquick::bindings::_sofaobject_

