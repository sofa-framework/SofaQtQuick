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
#include <SofaQtQuickGUI/Bindings/SofaData.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>

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
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(sofaqtquick::bindings::SofaData* positionData READ positionData WRITE setPositionData NOTIFY positionDataChanged)
    Q_PROPERTY(sofaqtquick::bindings::SofaData* orientationData READ orientationData WRITE setOrientationData NOTIFY orientationDataChanged)
    Q_PROPERTY(sofaqtquick::bindings::SofaData* scaleData READ scaleData WRITE setScaleData NOTIFY scaleDataChanged)
    Q_PROPERTY(sofaqtquick::bindings::SofaBase* sofaObject READ getSofaObject WRITE setSofaObject NOTIFY sofaObjectChanged)

    Q_PROPERTY(int particleIndex READ particleIndex WRITE setParticleIndex NOTIFY particleIndexChanged)
    Q_PROPERTY(QVector3D particlePosition READ particlePosition WRITE setParticlePosition NOTIFY particlePositionChanged)

    Q_CLASSINFO("DefaultProperty", "manipulators")

public:
    bool visible() const {return myVisible;}
    QString name() const {return myName;}
    void setVisible(bool newVisible);
    void setName(const QString& newName);

    sofaqtquick::bindings::SofaBase* getSofaObject();
    void setSofaObject(sofaqtquick::bindings::SofaBase* sofaObject);

    const QVector3D particlePosition();
    void setParticlePosition(const QVector3D particlePosition);

    int particleIndex();
    void setParticleIndex(const int particleIndex);

    const QVector3D position() const;
    void setPosition(const QVector3D& newPosition);
    void setPositionData(sofaqtquick::bindings::SofaData* newPosition);
    sofaqtquick::bindings::SofaData* positionData();

    const QQuaternion orientation() const;
    const QVector3D eulerOrientation() const;
    void setOrientation(const QQuaternion& newOrientation);
    void setOrientationData(sofaqtquick::bindings::SofaData* newOrientation);
    sofaqtquick::bindings::SofaData* orientationData();

    const QVector3D scale() const;
    void setScale(const QVector3D& newScale);
    void setScaleData(sofaqtquick::bindings::SofaData* newScale);
    sofaqtquick::bindings::SofaData* scaleData();


    Q_INVOKABLE void setOrientation(const QVector3D& newOrientation);

    Q_INVOKABLE QVector3D right() const;
    Q_INVOKABLE QVector3D up() const;
    Q_INVOKABLE QVector3D dir() const;

    Q_INVOKABLE QVector3D applyModelToPoint(const QVector3D& point) const;
    Q_INVOKABLE QVector3D applyModelToVector(const QVector3D& vector) const;


public slots:
    virtual void draw(const SofaViewer& viewer) const;
    virtual void pick(const SofaViewer& viewer) const;

    void onValueChanged(const QVariant& newValue);

signals:
    void visibleChanged(bool newVisible);
    void nameChanged(QString newName);
    void positionChanged(const QVector3D newPosition);
    void orientationChanged(const QQuaternion newOrientation);
    void scaleChanged(const QVector3D newScale);
    void particleIndexChanged(int newParticleIndex);
    void particlePositionChanged(const QVector3D newParticlePosition);

    void sofaObjectChanged(sofaqtquick::bindings::SofaBase* newSofaObject);
    void positionDataChanged(const sofaqtquick::bindings::SofaData* newPosition);
    void orientationDataChanged(const sofaqtquick::bindings::SofaData* newOrientation);
    void scaleDataChanged(const sofaqtquick::bindings::SofaData* newScale);

private:
    bool                             myVisible;
    QString                          myName;
    sofaqtquick::bindings::SofaData* myPosition;
    sofaqtquick::bindings::SofaData* myOrientation;
    sofaqtquick::bindings::SofaData* myScale;    

    int                              myParticleIndex;

    sofaqtquick::bindings::SofaBase* mySofaObject;
    sofa::defaulttype::Vec3d m_oldTranslation;
};

}  // namespace sofaqtquick


#endif // MANIPULATOR_H
