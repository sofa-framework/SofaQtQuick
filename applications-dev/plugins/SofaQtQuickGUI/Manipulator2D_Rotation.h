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

#ifndef MANIPULATOR2D_ROTATION_H
#define MANIPULATOR2D_ROTATION_H

#include "SofaQtQuickGUI.h"
#include "Manipulator.h"

#include <QObject>

namespace sofa
{

namespace qtquick
{

/// \class A view space manipulator allowing us to rotate things around the X or Y axis
class SOFA_SOFAQTQUICKGUI_API Manipulator2D_Rotation : public Manipulator
{
    Q_OBJECT

public:
    explicit Manipulator2D_Rotation(QObject* parent = 0);
    ~Manipulator2D_Rotation();

public:
    Q_INVOKABLE void setMark(float fromAngle, float toAngle);
    Q_INVOKABLE void unsetMark();

public slots:
    virtual void draw(const SofaViewer& viewer) const;
    virtual void pick(const SofaViewer& viewer) const;

private:
    void internalDraw(const SofaViewer& viewer, bool isPicking = false) const;

private:
    bool  myDisplayMark;
    float myFromMarkAngle;
    float myToMarkAngle;

};

}

}

#endif // MANIPULATOR2D_ROTATION_H
