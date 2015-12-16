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

#ifndef SOFA_COMPONENT_H
#define SOFA_COMPONENT_H

#include "SofaQtQuickGUI.h"
#include "SofaData.h"
#include <sofa/simulation/common/Simulation.h>

namespace sofa
{

namespace qtquick
{

class SofaScene;

/// QtQuick wrapper for a Sofa component (i.e baseObject / baseNode), allowing us to share a component in a QML context
class SOFA_SOFAQTQUICKGUI_API SofaComponent : public QObject
{
    Q_OBJECT

    friend class SofaData;

public:
    SofaComponent(const SofaScene* scene, const sofa::core::objectmodel::Base* base);
    SofaComponent(const SofaComponent& sceneComponent);

public:
    Q_PROPERTY(QString name READ name)

public:
    QString name() const;

public:
    Q_INVOKABLE bool isSame(SofaComponent* sceneComponent);
    Q_INVOKABLE sofa::qtquick::SofaData* getComponentData(const QString& name) const;

public:
    sofa::core::objectmodel::Base* base();
    const sofa::core::objectmodel::Base* base() const;

    const SofaScene* scene() const;

private:
    const SofaScene*                                    myScene;
    mutable const sofa::core::objectmodel::Base*    myBase;
};

}

}

#endif // SOFA_COMPONENT_H
