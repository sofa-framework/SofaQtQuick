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

#include <SofaQtQuickGUI/config.h>

#include <QObject>
#include <QQmlListProperty>
#include <QVector3D>
#include <QQuaternion>

namespace sofaqtquick
{

class SofaViewer;

/// \class An abstract manipulator allowing us to apply transformation on things
class SOFA_SOFAQTQUICKGUI_API Manipulator : public QObject
{
    Q_OBJECT

public:
    explicit Manipulator(QObject* parent = nullptr);
    ~Manipulator();

public:
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale NOTIFY scaleChanged)

    Q_CLASSINFO("DefaultProperty", "manipulators")

public:
    bool visible() const {return myVisible;}
    void setVisible(bool newVisible);

    const QVector3D& position() const {return myPosition;}
    void setPosition(const QVector3D& newPosition);

    const QQuaternion& orientation() const {return myOrientation;}
    void setOrientation(const QQuaternion& newOrientation);
    QVector3D eulerOrientation() const {return myOrientation.toEulerAngles();}
    void setEulerOrientation(const QVector3D& newOrientation);

    const QVector3D& scale() const {return myScale;}
    void setScale(const QVector3D& newScale);

    Q_INVOKABLE QVector3D right() const;
    Q_INVOKABLE QVector3D up() const;
    Q_INVOKABLE QVector3D dir() const;

    Q_INVOKABLE QVector3D applyModelToPoint(const QVector3D& point) const;
    Q_INVOKABLE QVector3D applyModelToVector(const QVector3D& vector) const;


public slots:
    virtual void draw(const SofaViewer& viewer) const;
    virtual void pick(const SofaViewer& viewer) const;

signals:
    void visibleChanged(bool newVisible);
    void positionChanged(const QVector3D& newPosition);
    void orientationChanged(const QQuaternion& newOrientation);
    void scaleChanged(const QVector3D& newScale);

private:
    bool                myVisible;
    QVector3D           myPosition;
    QQuaternion         myOrientation;
    QVector3D           myScale;

};

}  // namespace sofaqtquick


#endif // MANIPULATOR_H
