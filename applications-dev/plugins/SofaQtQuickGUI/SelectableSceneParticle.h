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

#ifndef SELECTABLE_SCENE_COMPONENT_PARTICLE_H
#define SELECTABLE_SCENE_COMPONENT_PARTICLE_H

#include "SofaQtQuickGUI.h"
#include "SelectableSceneComponent.h"

namespace sofa
{

namespace qtquick
{

/// QtQuick wrapper for a selectable object representing a SceneComponent particle (a dof)
class SOFA_SOFAQTQUICKGUI_API SelectableSceneParticle : public SelectableSceneComponent
{
    Q_OBJECT

public:
    SelectableSceneParticle(const SceneComponent& sceneComponent, int particleIndex);
    ~SelectableSceneParticle();

public:
    Q_PROPERTY(int particleIndex READ particleIndex)

public:
    int particleIndex() const {return myParticleIndex;}

private:
    int myParticleIndex;

};

}

}

#endif // SELECTABLE_SCENE_COMPONENT_PARTICLE_H
