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

#ifndef MANIPULATOR3D_TRANSLATION_H
#define MANIPULATOR3D_TRANSLATION_H

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include "Manipulator.h"

#include <QObject>

namespace sofa
{

namespace qtquick
{

/// \class An object or world space manipulator allowing us to translate things along X, Y and / or the Z axis
class SOFA_SOFAQTQUICKGUI_API Manipulator3D_Translation : public Manipulator
{
    Q_OBJECT

public:
    explicit Manipulator3D_Translation(QObject* parent = 0);
    ~Manipulator3D_Translation();

public:
    Q_PROPERTY(QString axis READ axis WRITE setAxis NOTIFY axisChanged)

public:
    QString axis() const {return myAxis;}
    void setAxis(QString newAxis);

signals:
    void axisChanged(QString newAxis);

public slots:
    virtual void draw(const SofaViewer& viewer) const;
    virtual void pick(const SofaViewer& viewer) const;

private:
    void internalDraw(const SofaViewer& viewer, bool isPicking = false) const;

private:
    QString myAxis;

};

}

}

#endif // MANIPULATOR3D_TRANSLATION_H
