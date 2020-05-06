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

#ifndef SELECTABLE_SOFA_COMPONENT_H
#define SELECTABLE_SOFA_COMPONENT_H

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Selectable.h>
#include <SofaQtQuickGUI/Bindings/SofaBaseObject.h>

namespace sofaqtquick
{

/// QtQuick wrapper for a selectable object representing a sofa component
class SOFA_SOFAQTQUICKGUI_API SelectableSofaComponent : public Selectable
{
    Q_OBJECT

public:
    SelectableSofaComponent(sofaqtquick::bindings::SofaBaseObject* sofaComponent);
    ~SelectableSofaComponent();

public:
    Q_PROPERTY(sofaqtquick::bindings::SofaBaseObject* sofaComponent READ sofaComponent)

public:
    sofaqtquick::bindings::SofaBaseObject* sofaComponent() const {return mySofaComponent;}

private:
    sofaqtquick::bindings::SofaBaseObject* mySofaComponent;

};

}

#endif // SELECTABLE_SOFA_COMPONENT_H
