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

#ifndef SELECTABLE_SCENE_COMPONENT_H
#define SELECTABLE_SCENE_COMPONENT_H

#include "SofaQtQuickGUI.h"
#include "Selectable.h"
#include "SceneComponent.h"

namespace sofa
{

namespace qtquick
{

/// QtQuick wrapper for a selectable object representing a SceneComponent
class SOFA_SOFAQTQUICKGUI_API SelectableSceneComponent : public Selectable
{
    Q_OBJECT

public:
    SelectableSceneComponent(const SceneComponent& sceneComponent);
    ~SelectableSceneComponent();

public:
    Q_PROPERTY(sofa::qtquick::SceneComponent* sceneComponent READ sceneComponent)

public:
    SceneComponent* sceneComponent() const {return mySceneComponent;}

private:
    SceneComponent* mySceneComponent;

};

}

}

#endif // SELECTABLE_SCENE_COMPONENT_H
