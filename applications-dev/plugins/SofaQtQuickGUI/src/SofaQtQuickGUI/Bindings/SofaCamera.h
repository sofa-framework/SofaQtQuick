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
#pragma once

#include <QObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QPoint>

#include <SofaBaseVisual/BaseCamera.h>

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
#include <SofaQtQuickGUI/Camera.h>
#include <SofaQtQuickGUI/Bindings/SofaBaseObject.h>

#include <sofa/defaulttype/Ray.h>
#include <sofa/simulation/VisualVisitor.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/simulation/MechanicalVisitor.h>

#include <QMetaType>

namespace sofaqtquick::binding
{

using sofaqtquick::SofaBaseScene;
using sofaqtquick::Camera;

/// \class SofaCamera expose a sofa::component::BaseCamera as a QtQuick Camera.
/// This allows to manipulate BaseCamera from QtQuick scripts.
class SOFA_SOFAQTQUICKGUI_API SofaCamera : public Camera
{
    Q_OBJECT

    Q_PROPERTY(sofaqtquick::bindings::SofaBaseObject* sofaComponent READ sofaComponent WRITE setSofaComponent  NOTIFY sofaComponentChanged)

signals:
    void sofaComponentChanged();
private slots:
    void handleSofaDataChange();

public:
    explicit SofaCamera(QObject* parent = nullptr);
    ~SofaCamera() override;

    sofaqtquick::bindings::SofaBaseObject* sofaComponent() const;
    void setSofaComponent(sofaqtquick::bindings::SofaBaseObject* sofaComponent);

    void setBaseCamera(sofa::component::visualmodel::BaseCamera* baseCamera);
    sofa::component::visualmodel::BaseCamera* getBaseCamera(){ return m_baseCamera; }

    //Camera API
    const QMatrix4x4& projection() const override;
    const QMatrix4x4& view() const override;
    const QMatrix4x4& model() const override;
    void setPixelResolution(double width, double height);

    Q_INVOKABLE QQuaternion orientation() const override;
    Q_INVOKABLE bool bindCameraFromScene(const SofaBaseScene* scene, const size_t index);

    Q_INVOKABLE QVector3D getOrigin(int x, int y)
    {
        sofa::defaulttype::Ray ray = m_baseCamera->screenPointToRay(sofa::defaulttype::Vec3(x,y,0.0));
        return QVector3D(ray.origin()[0], ray.origin()[1], ray.origin()[2]);
    }
    Q_INVOKABLE QVector3D getDirection(int x, int y)
    {
        std::cout << "22222 Camera Direction" << std::endl;
        sofa::defaulttype::Ray ray = m_baseCamera->screenPointToRay(sofa::defaulttype::Vec3(x,y,1.0));
        return QVector3D(ray.direction()[0], ray.direction()[1], ray.direction()[2]);
    }

    Q_INVOKABLE sofaqtquick::bindings::SofaBaseObject* pickMechanicalState(int x, int y)
    {
        sofa::defaulttype::Ray ray = m_baseCamera->screenPointToRay(sofa::defaulttype::Vec3(x,y,1000));


        sofa::simulation::MechanicalPickParticlesVisitor visitor(sofa::core::ExecParams::defaultInstance(),
                                                                 ray.origin(),
                                                                 ray.direction(),
                                                                 QVector3D(-103, -15, -15).length() / 76.0,
                                                                 0.001);


        visitor.execute(m_baseCamera->getContext()->getRootContext());

        if(!visitor.particles.empty())
        {
            sofa::core::behavior::BaseMechanicalState* mstate;
            unsigned int indexCollisionElement;
            sofa::defaulttype::Vector3 point;
            SReal rayLength;
            visitor.getClosestParticle( mstate, indexCollisionElement, point, rayLength );
            typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3Types> MechanicalObject3;

            MechanicalObject3* mechanicalObject = dynamic_cast<MechanicalObject3*>(mstate);

            std::cout <<  "got a Particle: position = " << point
                       << " mstate: " << mstate->getName()
                       << " rayLength = " << rayLength << std::endl;
            return new sofaqtquick::bindings::SofaBaseObject(sofa::core::objectmodel::BaseObject::SPtr(mechanicalObject));
        }
        return nullptr;
    }
private:
    sofaqtquick::bindings::SofaBaseObject* m_sofaComponent {nullptr};
    mutable sofa::component::visualmodel::BaseCamera* m_baseCamera {nullptr};
};

}

