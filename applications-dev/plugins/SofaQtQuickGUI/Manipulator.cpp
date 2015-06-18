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
    myPosition(),
    myOrientation(),
    myScale(1.0f, 1.0f, 1.0f)
{

}

Manipulator::~Manipulator()
{

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

void Manipulator::draw(const Viewer& viewer) const
{
    // a base manipulator has no visual representation
}

}

}
