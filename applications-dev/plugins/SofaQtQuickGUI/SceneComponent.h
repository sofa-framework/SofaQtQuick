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

#ifndef SCENE_COMPONENT_H
#define SCENE_COMPONENT_H

#include "SofaQtQuickGUI.h"
#include "SceneData.h"
#include <sofa/simulation/common/Simulation.h>

namespace sofa
{

namespace qtquick
{

class Scene;

/// QtQuick wrapper for a Sofa component (i.e baseObject / baseNode), allowing us to share a component in a QML context
class SOFA_SOFAQTQUICKGUI_API SceneComponent : public QObject
{
    Q_OBJECT

    friend class SceneData;

public:
    SceneComponent(const Scene* scene, const sofa::core::objectmodel::Base* base);
    SceneComponent(const SceneComponent& sceneComponent);

public:
    Q_PROPERTY(QString name READ name)

public:
    QString name() const;

public:
    Q_INVOKABLE bool isSame(SceneComponent* sceneComponent);
    Q_INVOKABLE sofa::qtquick::SceneData* getComponentData(const QString& name) const;

public:
    sofa::core::objectmodel::Base* base();
    const sofa::core::objectmodel::Base* base() const;

    const Scene* scene() const;

private:
    const Scene*                                    myScene;
    mutable const sofa::core::objectmodel::Base*    myBase;
};

}

}

#endif // SCENE_COMPONENT_H
