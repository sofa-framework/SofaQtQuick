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

#include <SofaQtQuickGUI/Camera.h>

#include <qqml.h>
#include <qmath.h>
#include <iostream>

namespace sofaqtquick
{

Camera::Camera(QObject* parent) : QObject(parent),
    myOrthographic(false),
    myOrthoLeft(-1.0),
    myOrthoRight(1.0),
    myOrthoBottom(-1.0),
    myOrthoTop(1.0),
    myPerspectiveFovY(55.0),
    myAspectRatio(16.0 / 9.0),
    myZNear(0.0001f),
    myZFar(1000000.f),
	myProjection(),
	myView(),
	myModel(),
    myTarget(),
	myProjectionDirty(true),
    myViewDirty(true)
{

}

Camera::~Camera()
{

}

const QMatrix4x4& Camera::projection() const
{
    if(myProjectionDirty) // update projection if needed
	{
		myProjection.setToIdentity();
        if(myOrthographic)
            myProjection.ortho(myOrthoLeft, myOrthoRight, myOrthoBottom, myOrthoTop, myZNear, myZFar);
        else
            myProjection.perspective((float) myPerspectiveFovY, (float) myAspectRatio, myZNear, myZFar);

        setProjectionDirty(false);
	}

	return myProjection;
}

const QMatrix4x4& Camera::view() const
{
    if(myViewDirty) // update view if needed
    {
        myView = model().inverted();
        setViewDirty(false);
    }

	return myView;
}

const QMatrix4x4& Camera::model() const
{
	return myModel;
}

QQuaternion Camera::orientation() const
{
    return  QQuaternion::fromRotationMatrix(myModel.normalMatrix());
}

double Camera::computeDepth(const QVector3D& wsPosition) const
{
    QVector4D csPosition = projection() * view() * QVector4D(wsPosition, 1.0);

    return csPosition.z() / csPosition.w();
}

QVector3D Camera::projectOnViewPlane(const QVector3D& wsPosition, double depth) const
{
    QVector4D csPosition = projection() * view() * QVector4D(wsPosition, 1.0);
    QVector4D nsPosition = csPosition / csPosition.w();

    csPosition = projection().inverted() * QVector4D(nsPosition.x(), nsPosition.y(), depth, 1.0);
    QVector4D vsPosition = csPosition / csPosition.w();

    return (model() * vsPosition).toVector3D();
}

QVector3D Camera::projectOnViewSpaceXAxis(const QVector3D& wsVector) const
{
    QVector3D right(Camera::right());

    return right * QVector3D::dotProduct(right, wsVector);
}

QVector3D Camera::projectOnViewSpaceYAxis(const QVector3D& wsVector) const
{
    QVector3D up(Camera::up());

    return up * QVector3D::dotProduct(up, wsVector);
}

void Camera::viewFromFront()
{
    QVector3D eye = myTarget + QVector3D( 0.0, 0.0, 1.0) * (myTarget - Camera::eye()).length();

    lookAt(eye, myTarget, QVector3D( 0.0, 1.0, 0.0));
}

void Camera::viewFromBack()
{
    QVector3D eye = myTarget + QVector3D( 0.0, 0.0,-1.0) * (myTarget - Camera::eye()).length();

    lookAt(eye, myTarget, QVector3D( 0.0, 1.0, 0.0));
}

void Camera::viewFromLeft()
{
    QVector3D eye = myTarget + QVector3D(-1.0, 0.0, 0.0) * (myTarget - Camera::eye()).length();

    lookAt(eye, myTarget, QVector3D( 0.0, 1.0, 0.0));
}

void Camera::viewFromRight()
{
    QVector3D eye = myTarget + QVector3D( 1.0, 0.0, 0.0) * (myTarget - Camera::eye()).length();

    lookAt(eye, myTarget, QVector3D( 0.0, 1.0, 0.0));
}

void Camera::viewFromTop()
{
    QVector3D eye = myTarget + QVector3D( 0.0, 1.0, 0.0) * (myTarget - Camera::eye()).length();

    lookAt(eye, myTarget, QVector3D( 0.0, 0.0,-1.0));
}

void Camera::viewFromBottom()
{
    QVector3D eye = myTarget + QVector3D( 0.0,-1.0, 0.0) * (myTarget - Camera::eye()).length();

    lookAt(eye, myTarget, QVector3D( 0.0, 0.0, 1.0));
}

void Camera::viewIsometric()
{
    QVector3D eye = QVector3D( 0.0, 0.0, 1.0);

    QMatrix4x4 rotation;
    rotation.rotate(45.0, 0.0, 1.0, 0.0);
    rotation.rotate(34.0,-1.0, 0.0, 0.0); // TODO: angle ?
    eye = rotation.map(eye);

    eye = myTarget + eye * (myTarget - Camera::eye()).length();

    lookAt(eye, myTarget, QVector3D( 0.0, 1.0, 0.0));
}

void Camera::move(double x, double y, double z)
{
    QVector3D translationVector(right() * x + up() * y + direction() * z);

    QMatrix4x4 translation;
    translation.translate(translationVector);

    myModel = translation * myModel;
    myTarget += translationVector;

    setViewDirty(true);
}

void Camera::turn(double angleAroundX, double angleAroundY, double angleAroundZ)
{
    QMatrix4x4 rotation;
    rotation.translate( target());
    rotation.rotate(angleAroundY, up());
    rotation.rotate(angleAroundX, right());
    rotation.rotate(angleAroundZ, direction()); // TODO: check rotation order
    rotation.translate(-target());

    myModel = rotation * myModel;

    setViewDirty(true);
}

void Camera::turnWorld(double angleAroundX, double angleAroundY, double angleAroundZ)
{
    QVector3D translationVector(-myTarget.x(), -myTarget.y(), -myTarget.z());

    QMatrix4x4 rotation;
    rotation.rotate(angleAroundY, up());
    rotation.rotate(angleAroundX, right());
    rotation.rotate(angleAroundZ, direction());

    QVector3D translationVector2(rotation * myTarget);

    // Translation
    QMatrix4x4 translation;
    translation.translate(translationVector);

    myModel = translation * myModel;
    myTarget += translationVector;

    // Rotation
    myModel = rotation * myModel;

    // Translation
    QMatrix4x4 translation2;
    translation2.translate(translationVector2);

    myModel = translation2 * myModel;
    myTarget += translationVector2;

    setViewDirty(true);
}

void Camera::zoom(double factor)
{
    return zoomWithBounds(factor, myZNear, myZFar * 0.5);
}

void Camera::zoomWithBounds(double factor, double min, double max)
{
    QVector3D translationVector(target() - eye());

    if(factor <= 0.0f)
    {
        translationVector = (target() - eye()) + (eye() - target()).normalized() * (max - std::numeric_limits<float>::epsilon());
    }
    else
    {
        factor = 1.0 / factor;
        translationVector *= (1.0 - factor);

        // clamp to bounds
        if((eye() + translationVector - target()).length() < min) // limit zoom-in to min
            translationVector = (target() - eye()) + (eye() - target()).normalized() * (min + std::numeric_limits<float>::epsilon());
        else if((eye() + translationVector - target()).length() > max) // limit zoom-out to max
            translationVector = (target() - eye()) + (eye() - target()).normalized() * (max - std::numeric_limits<float>::epsilon());
    }

    QMatrix4x4 translation;
    translation.translate(translationVector);

    myModel = translation * myModel;

    setViewDirty(true);

    if(orthographic())
        computeOrthographic();
}

void Camera::setOrthoLeft(double left)
{
    myOrthoLeft = left;

    setProjectionDirty(true);
}

void Camera::setOrthoRight(double right)
{
    myOrthoRight = right;

    setProjectionDirty(true);
}

void Camera::setOrthoBottom(double bottom)
{
    myOrthoBottom = bottom;

    setProjectionDirty(true);
}

void Camera::setOrthoTop(double top)
{
    myOrthoTop = top;

    setProjectionDirty(true);
}

void Camera::setPerspectiveFovY(double fovY)
{
    if(fovY == myPerspectiveFovY)
		return;

    myPerspectiveFovY = fovY;

    setProjectionDirty(true);
}

void Camera::setAspectRatio(double aspectRatio)
{
    if(aspectRatio == myAspectRatio)
		return;

    myAspectRatio = aspectRatio;

    setProjectionDirty(true);

    if(orthographic())
        computeOrthographic();
}

void Camera::lookAt(const QVector3D& eye, const QVector3D& target, const QVector3D& up)
{
    myView.setToIdentity();
    myView.lookAt(eye, target, up);
    myModel = myView.inverted();

    myTarget = target;

    setViewDirty(false);
}

void Camera::fit(QVector3D min, QVector3D max, float radiusFactor)
{
    if(min == max)
    {
        min -= QVector3D(0.5f, 0.5f, 0.5f);
        max += QVector3D(0.5f, 0.5f, 0.5f);
    }
    else if(min.x() > max.x() || min.y() > max.y() || min.z() > max.z())
    {
        min = QVector3D(-0.5f, -0.5f, -0.5f);
        max = QVector3D( 0.5f,  0.5f,  0.5f);
    }

    myTarget = (min + max) * 0.5;
	QVector3D diagonal = max - min;
	double radius = diagonal.length();

    double distance = radiusFactor * radius / qTan(myPerspectiveFovY * M_PI / 180.0);
    if(distance < 0.0001 || !(distance == distance)) // return if incorrect value, i.e < epsilon or nan
        return;

    setZNear(qMax(0.01, distance - radius * 100));
    setZFar(distance + radius * 100);

    if(!(myTarget == myTarget)) // wtf?
        myTarget = QVector3D(0.0f, 0.0f, 0.0f);

    QVector3D direction = Camera::direction();
    QVector3D up = Camera::up();
    if(!(direction == direction) || !(up == up))
    {
        direction = QVector3D(0.0f, 0.0f, -1.0f);
        up = QVector3D(0.0f, 1.0f, 0.0f);
    }

    QVector3D eye = myTarget - direction * distance;

    myView.setToIdentity();
    myView.lookAt(eye, myTarget, up);
    myModel = myView.inverted();

    setViewDirty(false);

    if(orthographic())
        computeOrthographic();
}


void Camera::adjustZRange(QVector3D min, QVector3D max, float radiusFactor)
{
    if(min == max)
    {
        min -= QVector3D(0.5f, 0.5f, 0.5f);
        max += QVector3D(0.5f, 0.5f, 0.5f);
    }
    else if(min.x() > max.x() || min.y() > max.y() || min.z() > max.z())
    {
        min = QVector3D(-0.5f, -0.5f, -0.5f);
        max = QVector3D( 0.5f,  0.5f,  0.5f);
    }

    QVector3D diagonal = max - min;
    double radius = diagonal.length();

    double distance = radiusFactor * radius / qTan(myPerspectiveFovY * M_PI / 180.0);
    if(distance < 0.0001 || !(distance == distance)) // return if incorrect value, i.e < epsilon or nan
        return;

    setZNear(qMax(0.01, distance - radius * 100));
    setZFar(distance + radius * 100);
}

float Camera::distanceFromTarget() const
{
    return (target() - eye()).length();
}

void Camera::setDistanceFromTarget(double distance)
{
    if(distance <= 0.0f)
        return;

    QVector3D translationVector(target() - eye() + (eye() - target()).normalized() * distance);

    QMatrix4x4 translation;
    translation.translate(translationVector);

    myModel = translation * myModel;

    setViewDirty(true);

    if(orthographic())
        computeOrthographic();
}

void Camera::alignCameraAxis()
{
    // Get camera right and up vectors
    QVector3D right(Camera::right());
    QVector3D up(Camera::up());

    // Compute nearest axis of right vector
    int nearAxisIndex = 0;
    QVector3D rightRef = computeNearestAxis(right,nearAxisIndex);
    int caseTested = nearAxisIndex;

    // Compute nearest axis of up vector
    QVector3D upRef = computeNearestAxis(up,nearAxisIndex,caseTested);

    // Update lookAt
    QVector3D eye = myTarget + QVector3D::crossProduct(rightRef,upRef) * (myTarget - Camera::eye()).length();
    lookAt(eye, myTarget, upRef);
}

void Camera::computeOrthographic()
{
    if(!orthographic())
        return;

    myOrthographic = false;
    myProjectionDirty = true;

    // compute the orthographic projection from the perspective one
    QMatrix4x4 perspectiveProj = Camera::projection();
    QMatrix4x4 perspectiveProjInv = perspectiveProj.inverted();

    myOrthographic = true;
    myProjectionDirty = true;

    QVector4D projectedTarget = perspectiveProj.map(view().map(QVector4D(target(), 1.0)));
    projectedTarget /= projectedTarget.w();

    QVector4D trCorner = perspectiveProjInv.map(QVector4D(1.0, 1.0, projectedTarget.z(), 1.0));
    trCorner /= trCorner.w();

    setOrthoLeft    (-trCorner.x());
    setOrthoRight   ( trCorner.x());
    setOrthoBottom  (-trCorner.y());
    setOrthoTop     ( trCorner.y());

    setProjectionDirty(true);
}

void Camera::computeModel()
{
    myView.setToIdentity();
    myView.lookAt(eye(), myTarget, up());
    myModel = myView.inverted();

    setViewDirty(false);
}

QVector3D Camera::computeNearestAxis(QVector3D axis, int& nearAxisIndex, int caseTested)
{
    QVector3D axisRef(0,0,0);
    double dotProductAxis=0.;

    // If we search the nearest axis of the first vector
    if(caseTested == -1)
    {
        // Init
        QVector3D iRef(1,0,0);
        dotProductAxis = QVector3D::dotProduct(axis, iRef);
        nearAxisIndex = 0;

        for(int j = 1; j<3; j++)
        {
            axisRef[j] = 1;
            if(std::abs(dotProductAxis) < std::abs(QVector3D::dotProduct(axis, axisRef)))
            {
                nearAxisIndex = j;
                dotProductAxis = QVector3D::dotProduct(axis, axisRef);
            }
            axisRef[j] = 0;
        }
    }

    // If we search the nearest axis of the second vector, remove the axis found for the first vector
    else
    {
        double iterations = 0;
        for(int j=0;j<3;j++)
        {
            // First iteration: init
            if(j!= caseTested && !iterations)
            {
                // Init axisRef
                axisRef[j]=1;
                nearAxisIndex = j;
                // Compute dotProduct
                dotProductAxis = QVector3D::dotProduct(axis, axisRef);
                axisRef[j] = 0;
                iterations++;
            }

            // Second iteration
            else if(j!= caseTested && iterations)
            {
                axisRef[j]=1;
                if(std::abs(dotProductAxis) < std::abs(QVector3D::dotProduct(axis, axisRef)))
                {
                    nearAxisIndex = j;
                    dotProductAxis = QVector3D::dotProduct(axis, axisRef);
                }
                axisRef[j] = 0;
            }
        }
    }

    // Return the nearest axis
    if(dotProductAxis >= 0)
      axisRef[nearAxisIndex] = 1;
    else
      axisRef[nearAxisIndex] = -1;

    return axisRef;
}

void Camera::setProjectionDirty(bool dirty) const
{
    myProjectionDirty = dirty;

    if(0 != receivers(SIGNAL(projectionChanged())))
    {
        if(myProjectionDirty)
            projection();
        else
            projectionChanged();
    }
}

void Camera::setViewDirty(bool dirty) const
{
    myViewDirty = dirty;

    if(0 != receivers(SIGNAL(modelViewChanged())))
    {
        if(myViewDirty)
            view();
        else
            modelViewChanged();
    }
}

}  // namespace sofaqtquick
