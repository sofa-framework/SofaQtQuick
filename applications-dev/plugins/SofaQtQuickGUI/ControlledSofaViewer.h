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

#ifndef CONTROLLEDSOFAVIEWER_H
#define CONTROLLEDSOFAVIEWER_H

#include "SofaQtQuickGUI.h"
#include "SofaViewer.h"

#include <QtQuick/QQuickFramebufferObject>
#include <QVector3D>
#include <QVector4D>
#include <QImage>
#include <QColor>
#include <QList>

#include <sofa/helper/io/Image.h>

namespace sofa
{

namespace qtquick
{

class SofaRenderer;
class SofaComponent;
class SofaScene;
class Camera;
class Manipulator;

class PickUsingRasterizationWorker;
class ScreenshotWorker;

/// \class Display a Sofa Scene in a QQuickFramebufferObject, and camera parameters comes from the sofa Scene

class SOFA_SOFAQTQUICKGUI_API ControlledSofaViewer : public SofaViewer
{
    Q_OBJECT

public:
    typedef SofaViewer Inherit;

    explicit ControlledSofaViewer(QQuickItem* parent = 0);
    ~ControlledSofaViewer();

public:

public:

private:

};

}

}

#endif // CONTROLLEDSOFAVIEWER_H
