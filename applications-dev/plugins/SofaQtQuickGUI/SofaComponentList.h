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

#ifndef SOFA_COMPONENTLIST_H
#define SOFA_COMPONENTLIST_H

#include "SofaQtQuickGUI.h"
#include "SofaComponent.h"

namespace sofa
{

namespace qtquick
{

class SofaScene;

class SOFA_SOFAQTQUICKGUI_API SofaComponentList : public QObject
{
    Q_OBJECT

public:
    SofaComponentList(SofaScene* sofaScene);
    SofaComponentList(SofaScene* sofaScene, const QList<SofaComponent*> componentList);
    SofaComponentList(const SofaComponentList& sofaComponentList);

    Q_INVOKABLE sofa::qtquick::SofaScene* sofaScene() const;

    //list methods
    Q_INVOKABLE void append(SofaComponent* sofaComponent) { myComponentList.append(sofaComponent); }
    Q_INVOKABLE sofa::qtquick::SofaComponent* at(int i) { return myComponentList.at(i); }
    Q_INVOKABLE int size() { return myComponentList.size(); }


public:
    QList<SofaComponent*> componentList();
    const QList<SofaComponent*> componentList() const;

    void operator=(const SofaComponentList& scl) 
    { 
        this->mySofaScene = scl.sofaScene();
        this->myComponentList = scl.componentList(); 
    }

private:
    SofaScene*               mySofaScene;
    QList<SofaComponent*>    myComponentList;

};

}

}

#endif // SOFA_COMPONENTLIST_H
