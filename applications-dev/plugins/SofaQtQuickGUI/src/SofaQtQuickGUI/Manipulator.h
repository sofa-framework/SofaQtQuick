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

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "SofaQtQuickGUI.h"

#include <QObject>
#include <QQmlListProperty>
#include <QVector3D>
#include <QQuaternion>

namespace sofa
{

namespace qtquick
{

class SofaViewer;

/// \class An abstract manipulator allowing us to apply transformation on things
class SOFA_SOFAQTQUICKGUI_API Manipulator : public QObject
{
    Q_OBJECT

    friend void appendManipulators(QQmlListProperty<Manipulator>* property, Manipulator* manipulator);
    friend int countManipulators(QQmlListProperty<Manipulator>* property);
    friend Manipulator* atManipulators(QQmlListProperty<Manipulator>* property, int index);
    friend void clearManipulators(QQmlListProperty<Manipulator>* property);

public:
    explicit Manipulator(QObject* parent = 0);
    ~Manipulator();

public:
    Q_PROPERTY(sofa::qtquick::Manipulator* rootManipulator READ rootManipulator NOTIFY rootManipulatorChanged)
    Q_PROPERTY(QQmlListProperty<sofa::qtquick::Manipulator> manipulators READ manipulators)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale NOTIFY scaleChanged)

    Q_CLASSINFO("DefaultProperty", "manipulators")

public:
    Manipulator* rootManipulator() const {return myRootManipulator;}
    void setRootManipulator(Manipulator* newRootManipulator);

    QQmlListProperty<sofa::qtquick::Manipulator> manipulators();

    bool visible() const {return myVisible;}
    void setVisible(bool newVisible);

    const QVector3D& position() const {return myPosition;}
    void setPosition(const QVector3D& newPosition);

    const QQuaternion& orientation() const {return myOrientation;}
    void setOrientation(const QQuaternion& newOrientation);

    const QVector3D& scale() const {return myScale;}
    void setScale(const QVector3D& newScale);

    Q_INVOKABLE QVector3D right() const;
    Q_INVOKABLE QVector3D up() const;
    Q_INVOKABLE QVector3D dir() const;

    Q_INVOKABLE QVector3D applyModelToPoint(const QVector3D& point) const;
    Q_INVOKABLE QVector3D applyModelToVector(const QVector3D& vector) const;

public:
    // DEPRECATED: use the SofaApplication singleton, it contains these functions
    Q_INVOKABLE QQuaternion quaternionFromEulerAngles(const QVector3D& eulerAngles);
    Q_INVOKABLE QVector3D quaternionToEulerAngles(const QQuaternion& quaternion);
    Q_INVOKABLE QVariantList quaternionToAxisAngle(const QQuaternion& quaternion); // return [QVector3D axis, float angle];
    Q_INVOKABLE QQuaternion quaternionFromAxisAngle(const QVector3D& axis, float angle);
    Q_INVOKABLE QQuaternion quaternionDifference(const QQuaternion& q0, const QQuaternion& q1);
    Q_INVOKABLE QQuaternion quaternionMultiply(const QQuaternion& q0, const QQuaternion& q1);
    //

public slots:
    virtual void draw(const SofaViewer& viewer) const;
    virtual void pick(const SofaViewer& viewer) const;

signals:
    void rootManipulatorChanged(Manipulator* newRootManipulator);
    void visibleChanged(bool newVisible);
    void positionChanged(const QVector3D& newPosition);
    void orientationChanged(const QQuaternion& newOrientation);
    void scaleChanged(const QVector3D& newScale);

private:
    Manipulator*        myRootManipulator;
    QList<Manipulator*> myManipulators;
    bool                myVisible;
    QVector3D           myPosition;
    QQuaternion         myOrientation;
    QVector3D           myScale;

};

}

}

#endif // MANIPULATOR_H
