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

#include "SofaCamera.h"

#include <qqml.h>
#include <qmath.h>
#include <iostream>

namespace sofa
{

namespace qtquick
{

SofaCamera::SofaCamera(QObject* parent)
    : Camera(parent)
    , m_sofaComponent(NULL)
    , m_baseCamera(NULL)
{
    connect(this, &SofaCamera::sofaComponentChanged, this, &SofaCamera::handleSofaDataChange);
}

SofaCamera::~SofaCamera()
{

}

qtquick::SofaComponent* SofaCamera::sofaComponent() const
{
    return m_sofaComponent;
}

void SofaCamera::setSofaComponent(qtquick::SofaComponent* sofaComponent)
{
    if (sofaComponent == m_sofaComponent)
        return;

    m_sofaComponent = sofaComponent;
    if (sofaComponent)
        m_sofaComponent = new qtquick::SofaComponent(*sofaComponent);

    sofaComponentChanged();
}

void SofaCamera::setBaseCamera(sofa::component::visualmodel::BaseCamera* baseCamera)
{
    m_baseCamera = baseCamera;
}

void SofaCamera::handleSofaDataChange()
{

    if (!m_sofaComponent)
        return;

    core::objectmodel::Base* baseComponent = m_sofaComponent->base();
    if (!baseComponent)
        return;
    
    //Here, list all potential camera
    QString type = QString::fromStdString(baseComponent->getTypeName());
    if (0 == type.compare("InteractiveCamera"))
        setBaseCamera(dynamic_cast<sofa::component::visualmodel::BaseCamera*>(baseComponent));
    else
        msg_error("SofaCamera") << "Type unknown";

}

const QMatrix4x4& SofaCamera::projection() const
{
    if (!m_baseCamera)
        return QMatrix4x4();
    
    double dmat[16];
    m_baseCamera->getProjectionMatrix(dmat);

    QMatrix4x4 qtproj(dmat[0], dmat[1], dmat[2], dmat[3]
        , dmat[4], dmat[5], dmat[6], dmat[7]
        , dmat[8], dmat[9], dmat[10], dmat[11]
        , dmat[12], dmat[13], dmat[14], dmat[15]);

    myProjection = qtproj;
        
    return myProjection;
}

const QMatrix4x4& SofaCamera::view() const
{
    if (!m_baseCamera)
        return QMatrix4x4();

    double dmat[16];
    m_baseCamera->getModelViewMatrix(dmat);
    QMatrix4x4 qtmodel(dmat[0], dmat[1], dmat[2], dmat[3]
        , dmat[4], dmat[5], dmat[6], dmat[7]
        , dmat[8], dmat[9], dmat[10], dmat[11]
        , dmat[12], dmat[13], dmat[14], dmat[15]);

    myView = qtmodel;

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


}

}
