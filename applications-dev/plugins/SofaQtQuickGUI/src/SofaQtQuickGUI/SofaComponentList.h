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

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>

namespace sofaqtquick
{

class SofaBaseScene;

/// \brief This class allows the use of a list of SofaBase in a QML file
/// as QList<SofaBase> is not usable in a QML file

class SOFA_SOFAQTQUICKGUI_API SofaBaseList : public QObject
{
    Q_OBJECT

public:
    SofaBaseList(SofaBaseScene* sofaScene);
    SofaBaseList(SofaBaseScene* sofaScene, const QList<sofaqtquick::bindings::SofaBase*> componentList);
    SofaBaseList(const SofaBaseList& SofaBaseList);

    /// \brief get the pointer to the Sofa Scene 
    Q_INVOKABLE sofaqtquick::SofaBaseScene* sofaScene() const;

    //List methods
    /// \brief append a SofaBase into the list
    Q_INVOKABLE void append(sofaqtquick::bindings::SofaBase* SofaBase) { myComponentList.append(SofaBase); }
    /// \brief retrieve a SofaBase from the list at index i
    Q_INVOKABLE sofaqtquick::bindings::SofaBase* at(int i) { return ( i < myComponentList.size() ) ? myComponentList.at(i) : nullptr; }
    /// \brief return the size of the list 
    Q_INVOKABLE int size() { return myComponentList.size(); }

public:
    QList<sofaqtquick::bindings::SofaBase*> componentList();
    const QList<sofaqtquick::bindings::SofaBase*> componentList() const;

    void operator=(const SofaBaseList& scl)
    { 
        this->mySofaScene = scl.sofaScene();
        this->myComponentList = scl.componentList(); 
    }

private:
    SofaBaseScene*               mySofaScene;
    QList<sofaqtquick::bindings::SofaBase*>    myComponentList;

};

}  // namespace sofaqtquick

#endif // SOFA_COMPONENTLIST_H
