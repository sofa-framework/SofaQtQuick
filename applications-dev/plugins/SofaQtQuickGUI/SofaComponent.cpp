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

#include <GL/glew.h>
#include "SofaComponent.h"
#include "SofaScene.h"

namespace sofa
{

namespace qtquick
{

using namespace sofa::core::objectmodel;

SofaComponent::SofaComponent(const SofaScene* scene, const sofa::core::objectmodel::Base* base) : QObject(),
    myScene(scene),
    myBase(base)
{

}

SofaComponent::SofaComponent(const SofaComponent& sceneComponent) : QObject(),
    myScene(sceneComponent.scene()),
    myBase(sceneComponent.base())
{

}

QString SofaComponent::name() const
{
    const Base* base = SofaComponent::base();
    if(!base)
        return QString("Invalid SofaComponent");

    return QString::fromStdString(base->getName());
}

bool SofaComponent::isSame(SofaComponent* sceneComponent)
{
    if(!sceneComponent)
        return false;

    // same wrapper => same component
    if(this == sceneComponent)
        return true;

    // same base object => same component
    if(base() == sceneComponent->base())
        return true;

    return false;
}

SofaData* SofaComponent::getComponentData(const QString& name) const
{
    sofa::core::objectmodel::BaseData* data = myBase->findData(name.toStdString());
    if(!data)
        return 0;

    return new SofaData(this, data);
}

Base* SofaComponent::base()
{
    return const_cast<Base*>(static_cast<const SofaComponent*>(this)->base());
}

const Base* SofaComponent::base() const
{
    // check object existence
    if(myScene && myBase)
        if(myScene->myBases.contains(myBase))
            return myBase;

    myBase = 0;
    return 0;
}

const SofaScene* SofaComponent::scene() const
{
    return myScene;
}

}

}
