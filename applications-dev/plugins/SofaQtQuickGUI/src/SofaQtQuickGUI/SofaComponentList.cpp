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

#include <SofaQtQuickGUI/SofaComponentList.h>
#include <SofaQtQuickGUI/SofaBaseScene.h>

#include <sofa/core/ObjectFactory.h>

#include <qqml.h>
#include <QQmlEngine>

namespace sofa
{

namespace qtquick
{

using namespace sofa::core::objectmodel;

SofaComponentList::SofaComponentList(SofaBaseScene* sofaScene)
    : QObject()
    , mySofaScene(sofaScene)
{

}

SofaComponentList::SofaComponentList(SofaBaseScene* sofaScene, const QList<SofaComponent*> baseList) 
    : QObject()
    , mySofaScene(sofaScene)
    , myComponentList(baseList)
{

}

SofaComponentList::SofaComponentList(const SofaComponentList& sofaComponentList) 
    : QObject()
    , mySofaScene(sofaComponentList.sofaScene())
    , myComponentList(sofaComponentList.componentList())
{

}

SofaBaseScene* SofaComponentList::sofaScene() const
{
    return mySofaScene;
}

QList<SofaComponent*> SofaComponentList::componentList()
{
    return myComponentList;
}

const QList<SofaComponent*> SofaComponentList::componentList() const
{
    return myComponentList;
}

}

}
