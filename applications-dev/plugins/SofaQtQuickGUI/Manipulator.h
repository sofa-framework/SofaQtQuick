#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include "SofaQtQuickGUI.h"

#include <QObject>
#include <QVector3D>
#include <QQuaternion>

namespace sofa
{

namespace qtquick
{

class Viewer;

class SOFA_SOFAQTQUICKGUI_API Manipulator : public QObject
{
    Q_OBJECT

public:
    explicit Manipulator(QObject* parent = 0);
    ~Manipulator();

public:
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QQuaternion orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale NOTIFY scaleChanged)

public:
    const QVector3D& position() const {return myPosition;}
    void setPosition(const QVector3D& newPosition);

    const QQuaternion& orientation() const {return myOrientation;}
    void setOrientation(const QQuaternion& newOrientation);

    const QVector3D& scale() const {return myScale;}
    void setScale(const QVector3D& newScale);

public slots:
    virtual void draw(const Viewer& viewer) const;

signals:
    void positionChanged(const QVector3D& newPosition);
    void orientationChanged(const QQuaternion& newOrientation);
    void scaleChanged(const QVector3D& newScale);

private:
    QVector3D   myPosition;
    QQuaternion myOrientation;
    QVector3D   myScale;

};

}

}

#endif // MANIPULATOR_H
