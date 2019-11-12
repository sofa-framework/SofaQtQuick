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

#ifndef SOFASCENELISTPROXY_H
#define SOFASCENELISTPROXY_H

#include "SofaSceneListModel.h"

namespace sofaqtquick
{

/// \class A Model of the sofa scene graph allowing us to show the graph in a ListView
class SofaSceneListProxy : public SofaSceneListModel
{
    Q_OBJECT

    Q_INVOKABLE void setDisabled(int modelRow, bool value);
    Q_INVOKABLE void setCollapsed(int modelRow, bool value);

public:
    SofaSceneListProxy(QObject* parent = 0);
    virtual ~SofaSceneListProxy();

};

}

#endif // SOFASCENELISTPROXY_H
