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
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>

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

            /// Returns the number of link in the SofaLink (in case of multilinks)
            Q_INVOKABLE unsigned int getSize();
            Q_INVOKABLE SofaBase* getLinkedBase(size_t index = 0);
            Q_INVOKABLE void setLinkedBase(SofaBase* linkedbase);
            Q_INVOKABLE void setLinkedPath(const QString& str);
            Q_INVOKABLE SofaData* getLinkedData(size_t index = 0);
            Q_INVOKABLE QString   getLinkedPath(size_t index = 0);

        private:
            BaseLink* m_self {nullptr};
        };
    }

    /// Import SofaData from its private namespace into the public one.
    using _sofalink_::SofaLink;
}


////////////////////////////////////// DECLARATION //////////////////////////////////////////
//TODO(dmarchal:04/04/2019) The following is deprecated.
namespace sofaqtquick
{

using sofaqtquick::bindings::SofaLink;

} // namespace sofaqtquick

