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
    myRootManipulator(this),
    myManipulators(),
    myVisible(true),
    myPosition(),
    myOrientation(),
    myScale(1.0f, 1.0f, 1.0f)
{

}

Manipulator::~Manipulator()
{

}

void Manipulator::setRootManipulator(Manipulator* newRootManipulator)
{
    if(!newRootManipulator)
        newRootManipulator = this;

    if(newRootManipulator == myRootManipulator)
        return;

    myRootManipulator = newRootManipulator;

    rootManipulatorChanged(newRootManipulator);
}

void appendManipulators(QQmlListProperty<Manipulator>* property, Manipulator* value)
{
    static_cast<QList<Manipulator*>*>(property->data)->append(value);
    value->setRootManipulator(static_cast<Manipulator*>(property->object)->rootManipulator());
}

int countManipulators(QQmlListProperty<Manipulator>* property)
{
    return static_cast<QList<Manipulator*>*>(property->data)->size();
}

Manipulator* atManipulators(QQmlListProperty<Manipulator>* property, int index)
{
    return static_cast<QList<Manipulator*>*>(property->data)->at(index);
}

void clearManipulators(QQmlListProperty<Manipulator>* property)
{
    QList<Manipulator*>& manipulators = *static_cast<QList<Manipulator*>*>(property->data);
    for(Manipulator* manipulator : manipulators)
        manipulator->setRootManipulator(manipulator);

    static_cast<QList<Manipulator*>*>(property->data)->clear();
}

QQmlListProperty<sofa::qtquick::Manipulator> Manipulator::manipulators()
{
    return QQmlListProperty<sofa::qtquick::Manipulator>(this, &myManipulators, appendManipulators, countManipulators, atManipulators, clearManipulators);
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

QVector3D Manipulator::quaternionToEulerAngles(const QQuaternion& quaternion)
{
    return quaternion.toEulerAngles();
}

QVariantList Manipulator::quaternionToAxisAngle(const QQuaternion& quaternion) // return [QVector3D axis, float angle];
{
    QVector3D axis;
    float angle = 0.0f;

    quaternion.getAxisAndAngle(&axis, &angle);

    return QVariantList() << QVariant::fromValue(axis) << QVariant::fromValue(angle);
}

QQuaternion Manipulator::quaternionFromAxisAngle(const QVector3D& axis, float angle)
{
    return QQuaternion::fromAxisAndAngle(axis, angle);
}

QQuaternion Manipulator::quaternionDifference(const QQuaternion& q0, const QQuaternion& q1)
{
    return q1 * q0.inverted();
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
