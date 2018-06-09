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

#ifndef SELECTABLE_H
#define SELECTABLE_H

#include "SofaQtQuickGUI.h"
#include <QObject>
#include <QVector3D>

namespace sofa
{

namespace qtquick
{

/// QtQuick abstract wrapper for a selectable object
class SOFA_SOFAQTQUICKGUI_API Selectable : public QObject
{
    Q_OBJECT

public:
    Selectable(QObject* parent = 0);
    ~Selectable();

public:
    Q_PROPERTY(QVector3D position READ position)

public:
    const QVector3D& position() const {return myPosition;}
    void setPosition(const QVector3D& position);

private:
    QVector3D myPosition;   ///< the position of the selected object

};

}

}

#endif // SELECTABLE_H
