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

#include <qqml.h>
#include <qmath.h>
#include <iostream>

#include <SofaQtQuickGUI/Helper/QMathExtensions.h>
using sofaqtquickgui::helper::QMath ;

#include <SofaQtQuickGUI/SofaScene.h>
using sofa::qtquick::SofaScene;

#include <SofaBaseVisual/BaseCamera.h>
using sofa::component::visualmodel::BaseCamera ;

#include "SofaCamera.h"


namespace sofaqtquick::binding
{

using sofa::core::objectmodel::Base;
using sofa::qtquick::SofaComponent;

SofaCamera::SofaCamera(QObject* parent)
    : Camera(parent)
    , m_sofaComponent(nullptr)
    , m_baseCamera(nullptr)
{
    connect(this, &SofaCamera::sofaComponentChanged, this, &SofaCamera::handleSofaDataChange);
}

SofaCamera::~SofaCamera()
{

}

SofaComponent* SofaCamera::sofaComponent() const
{
    return m_sofaComponent;
}

void SofaCamera::setSofaComponent(SofaComponent* sofaComponent)
{
    if (sofaComponent == m_sofaComponent)
        return;

    m_sofaComponent = sofaComponent;
    if (sofaComponent)
        m_sofaComponent = new SofaComponent(*sofaComponent);

    sofaComponentChanged();
}

void SofaCamera::setBaseCamera(BaseCamera* baseCamera)
{
    m_baseCamera = baseCamera;
}

void SofaCamera::handleSofaDataChange()
{

    if (!m_sofaComponent)
        return;

    Base* baseComponent = m_sofaComponent->base();
    if (!baseComponent)
        return;
    
    BaseCamera* camera = dynamic_cast<BaseCamera*>(baseComponent) ;
    if (camera)
        setBaseCamera(camera);
    else
        msg_error("SofaCamera") << "Type unknown";
}

const QMatrix4x4& SofaCamera::projection() const
{
    if (!m_baseCamera)
        return Camera::projection();
    // return QMath::Identity<QMatrix4x4>();
    
    double dmat[16];
    m_baseCamera->getProjectionMatrix(dmat);
    QMath::setMatrixFrom(myProjection, dmat);

    return myProjection;
}

const QMatrix4x4& SofaCamera::view() const
{
    if (!m_baseCamera)
        return Camera::view();

    if (!m_baseCamera)
        return QMath::Identity<QMatrix4x4>();


    double dmat[16];
    m_baseCamera->getModelViewMatrix(dmat);
    QMath::setMatrixFrom(myView, dmat);

    return myView;
}

const QMatrix4x4& SofaCamera::model() const
{
    myModel = myView.inverted();

    return myModel;
}

QQuaternion SofaCamera::orientation() const
{
    return  QQuaternion::fromRotationMatrix(myModel.normalMatrix());
}

bool SofaCamera::bindCameraFromScene(const SofaScene* scene, const size_t index)
{
    auto& node = scene->sofaRootNode();
    auto cameras = node->getTreeObjects<BaseCamera>();
    if(index < cameras.size())
        return false;

    m_baseCamera = cameras[index];
    return true;
}


}
