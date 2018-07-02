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

#ifndef SofaCamera_H
#define SofaCamera_H

#include <QObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QPoint>

#include <SofaBaseVisual/BaseCamera.h>

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include <SofaQtQuickGUI/SofaComponent.h>
#include <SofaQtQuickGUI/Camera.h>

namespace sofa
{

namespace qtquick
{

/// \class SofaCamera expose a sofa::component::BaseCamera as a QtQuick Camera.
/// This allows to manipulate BaseCamera from QtQuick scripts.
class SOFA_SOFAQTQUICKGUI_API SofaCamera : public Camera
{
    Q_OBJECT

    Q_PROPERTY(sofa::qtquick::SofaComponent* sofaComponent READ sofaComponent WRITE setSofaComponent  NOTIFY sofaComponentChanged)

signals:
    void sofaComponentChanged();
private slots:
    void handleSofaDataChange();

public:
    explicit SofaCamera(QObject* parent = 0);
    ~SofaCamera();
    sofa::qtquick::SofaComponent* sofaComponent() const;
    void setSofaComponent(sofa::qtquick::SofaComponent* sofaComponent);

    void setBaseCamera(sofa::component::visualmodel::BaseCamera* baseCamera);
    sofa::component::visualmodel::BaseCamera* getBaseCamera(){ return m_baseCamera; }

    //Camera API
    const QMatrix4x4& projection() const override;
    const QMatrix4x4& view() const override;
    const QMatrix4x4& model() const override;

    Q_INVOKABLE QQuaternion orientation() const override;

private:
    sofa::qtquick::SofaComponent* m_sofaComponent {nullptr};
    mutable sofa::component::visualmodel::BaseCamera* m_baseCamera {nullptr};

};

}

}

#endif // SofaCamera_H
