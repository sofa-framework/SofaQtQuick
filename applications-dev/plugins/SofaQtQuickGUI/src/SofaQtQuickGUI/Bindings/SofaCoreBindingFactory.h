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
#pragma once

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Bindings/SofaBaseObject.h>
#include <map>
#include <functional>

namespace sofaqtquick::bindings
{
using sofaqtquick::bindings::SofaBaseObject;

class SofaCoreBindingFactory
{
public:
    typedef std::function<SofaBase*(sofa::core::objectmodel::Base*)> downcastFct;
    static void registerType(const std::string& className, downcastFct f)
    {
        m_downcasts[className] = f;
    }

    static SofaBase* wrap(sofa::core::objectmodel::Base* obj)
    {
        auto f = m_downcasts.find(obj->getClassName());
        if( f != m_downcasts.end() )
        {
            return f->second(obj);
        }
        if(obj->toBaseObject())
            return new SofaBaseObject(obj->toBaseObject());

        return new SofaBase(obj);
    }

private:
    static std::map<std::string, downcastFct> m_downcasts;
};
} /// sofaqtquick::bindings

