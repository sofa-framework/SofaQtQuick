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

#ifndef SELECTABLE_SOFA_PARTICLE_H
#define SELECTABLE_SOFA_PARTICLE_H

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include <SofaQtQuickGUI/SelectableSofaComponent.h>

namespace sofa
{

namespace qtquick
{

/// QtQuick wrapper for a selectable object representing a sofa particle (a dof)
class SOFA_SOFAQTQUICKGUI_API SelectableSofaParticle : public SelectableSofaComponent
{
    Q_OBJECT

public:
    SelectableSofaParticle(const SofaComponent& sofaComponent, int particleIndex);
    ~SelectableSofaParticle();

public:
    Q_PROPERTY(int particleIndex READ particleIndex)

public:
    int particleIndex() const {return myParticleIndex;}

private:
    int myParticleIndex;

};

}

}

#endif // SELECTABLE_SOFA_PARTICLE_H
