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

namespace sofaqtquick
{

using namespace sofa::core::objectmodel;
using namespace sofaqtquick::bindings;

SofaBaseList::SofaBaseList(SofaBaseScene* sofaScene)
    : QObject()
    , mySofaScene(sofaScene)
{

}

SofaBaseList::SofaBaseList(SofaBaseScene* sofaScene, const QList<SofaBase*> baseList)
    : QObject()
    , mySofaScene(sofaScene)
    , myComponentList(baseList)
{

}

SofaBaseList::SofaBaseList(const SofaBaseList& SofaBaseList)
    : QObject()
    , mySofaScene(SofaBaseList.sofaScene())
    , myComponentList(SofaBaseList.componentList())
{

}

SofaBaseScene* SofaBaseList::sofaScene() const
{
    return mySofaScene;
}

QList<SofaBase*> SofaBaseList::componentList()
{
    return myComponentList;
}

const QList<SofaBase*> SofaBaseList::componentList() const
{
    return myComponentList;
}

}  // sofaqtquick

