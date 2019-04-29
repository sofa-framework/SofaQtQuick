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
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>

/////////////////////////////////////// DECLARTAION /////////////////////////////////////////
namespace sofaqtquick::bindings
{
    namespace _sofalink_
    {
        using sofa::core::objectmodel::BaseLink;

        class SofaLink : public QObject
        {
            Q_OBJECT

        public:
            SofaLink(BaseLink* self);

            /// connect the link to an object pointed by the path.
            Q_INVOKABLE bool setValueString(const QString& path);
            Q_INVOKABLE QString getValueString();

        private:
            BaseLink* m_self {nullptr};
        };
    }

    /// Import SofaData from its private namespace into the public one.
    using _sofalink_::SofaLink;
}


////////////////////////////////////// DECLARATION //////////////////////////////////////////
//TODO(dmarchal:04/04/2019) The following is deprecated.
namespace sofa
{

namespace qtquick
{

using sofaqtquick::bindings::SofaLink;

} /// qtquick
} /// sofa

