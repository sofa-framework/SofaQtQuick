/*
Copyright 2018, INRIA

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

#ifndef SOFAQTQUICK_CAMERAVIEW_H
#define SOFAQTQUICK_CAMERAVIEW_H

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/SofaViewer.h>

#include <QtQuick/QQuickFramebufferObject>
#include <QVector3D>
#include <QVector4D>
#include <QImage>
#include <QColor>
#include <QList>

#include <sofa/helper/io/Image.h>

////////////////////////// FORWARD DECLARATION ///////////////////////////
namespace sofaqtquick::binding
{
        class SofaCamera;
}

//////////////////////////////// DEFINITION /////////////////////////////
namespace sofa
{
namespace qtquick
{

using sofaqtquick::binding::SofaCamera;

/// \class Display a Sofa Scene in a QQuickFramebufferObject using the camera parameters
/// that comes from the Sofa Scene.
class SOFA_SOFAQTQUICKGUI_API CameraView : public SofaViewer
{
    Q_OBJECT

public:
    typedef SofaViewer Parent;

    explicit CameraView(QQuickItem* parent = nullptr);
    ~CameraView() override;

protected:
    virtual void internalRender(int width, int height) const override ;
};

}
}

#endif // SOFAQTQUICK_CAMERAVIEW_H
