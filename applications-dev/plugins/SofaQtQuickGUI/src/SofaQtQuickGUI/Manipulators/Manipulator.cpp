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

#include <SofaQtQuickGUI/Manipulators/Manipulator.h>
#include <SofaQtQuickGUI/SofaViewer.h>

#include <QMatrix4x4>
#include <QDebug>

namespace sofaqtquick
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

QQmlListProperty<sofaqtquick::Manipulator> Manipulator::manipulators()
{
    return QQmlListProperty<sofaqtquick::Manipulator>(this, &myManipulators, appendManipulators, countManipulators, atManipulators, clearManipulators);
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

QVector3D Manipulator::right() const
{
    return myOrientation.rotatedVector(QVector3D(1.0f, 0.0f, 0.0f));
}

QVector3D Manipulator::up() const
{
    return myOrientation.rotatedVector(QVector3D(0.0f, 1.0f, 0.0f));
}

QVector3D Manipulator::dir() const
{
    return myOrientation.rotatedVector(QVector3D(0.0f, 0.0f, 1.0f));
}

QVector3D Manipulator::applyModelToPoint(const QVector3D& point) const
{
    QMatrix4x4 model(myOrientation.toRotationMatrix());
    model.setColumn(3, QVector4D(myPosition, 1.0f));

    return model.map(point);
}

QVector3D Manipulator::applyModelToVector(const QVector3D& vector) const
{
    return QMatrix4x4(myOrientation.toRotationMatrix()).mapVector(vector);
}

void Manipulator::draw(const SofaViewer& /*viewer*/) const
{
    // a base manipulator has no visual representation
}

void Manipulator::pick(const SofaViewer& viewer) const
{
    draw(viewer);
}

}  // namespace sofaqtquick
