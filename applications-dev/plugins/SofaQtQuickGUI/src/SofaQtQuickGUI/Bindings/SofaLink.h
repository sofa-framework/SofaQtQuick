/*
Copyright 2015, Anatoscope

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

#ifndef SOFA_LINK_H
#define SOFA_LINK_H

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
//#include <SofaQtQuickGUI/SofaScene.h>

////////////////////////////// FORWARD DECLARATION //////////////////////////////////////////
namespace sofa {
    namespace core {
        namespace objectmodel {
            class Base;
            class BaseLink ;
        }
    }
    namespace qtquick {
        class SofaComponent ;
        class SofaScene ;
    }
}

////////////////////////////////////// DECLARATION //////////////////////////////////////////
namespace sofa
{

namespace qtquick
{

namespace _sofalink_h_
{

using sofa::core::objectmodel::Base;
using sofa::core::objectmodel::BaseLink ;

/// QtQuick wrapper for a sofa link (i.e BaseLink), allowing us to share a component link
/// in a QML context
class SOFA_SOFAQTQUICKGUI_API SofaLink : public QObject
{
    Q_OBJECT

public:
    SofaLink(const SofaComponent* sofaComponent, BaseLink* link);
    SofaLink(SofaScene* sofaScene, const Base* base, BaseLink *link);
    SofaLink(const SofaLink& sceneData);

    Q_INVOKABLE SofaComponent* sofaComponent() const;
    Q_INVOKABLE bool setValue(const QString& path);

private:
    SofaComponent*     m_component {nullptr};
    BaseLink*          m_link {nullptr};
};

} /// _sofalink_h_

using sofa::qtquick::_sofalink_h_::SofaLink ;

} /// qtquick
} /// sofa

#endif // SOFA_LINK_H
