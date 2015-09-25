#include "Manipulator.h"
#include "Viewer.h"

#include <GL/glew.h>
#include <QMatrix4x4>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

Manipulator::Manipulator(QObject* parent) : QObject(parent),
    myVisible(true),
    myPosition(),
    myOrientation(),
    myScale(1.0f, 1.0f, 1.0f)
{

}

Manipulator::~Manipulator()
{

}

void Manipulator::setVisible(bool newVisible)
{
    if(newVisible == myVisible)
        return;

    myVisible = newVisible;

    visibleChanged(newVisible);
}

void Manipulator::setPosition(const QVector3D& newPosition)
{
    if(newPosition == myPosition)
        return;

    myPosition = newPosition;

    positionChanged(newPosition);
}

void Manipulator::setOrientation(const QQuaternion& newOrientation)
{
    if(newOrientation == myOrientation)
        return;

    myOrientation = newOrientation;

    orientationChanged(newOrientation);
}

void Manipulator::setScale(const QVector3D& newScale)
{
    if(newScale == myScale)
        return;

    myScale = newScale;

    scaleChanged(newScale);
}

QQuaternion Manipulator::quaternionFromAxisAngle(const QVector3D& axis, float angle)
{
    return QQuaternion::fromAxisAndAngle(axis, angle);
}

QQuaternion Manipulator::quaternionMultiply(const QQuaternion& q0, const QQuaternion& q1)
{
    return q0 * q1;
}

void Manipulator::draw(const Viewer& /*viewer*/) const
{
    // a base manipulator has no visual representation
}

void Manipulator::pick(const Viewer& viewer) const
{
    draw(viewer);
}

}

}
