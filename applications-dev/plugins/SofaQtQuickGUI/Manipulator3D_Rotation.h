#ifndef MANIPULATOR3D_ROTATION_H
#define MANIPULATOR3D_ROTATION_H

#include "SofaQtQuickGUI.h"
#include "Manipulator.h"

#include <QObject>
#include <QVector3D>

namespace sofa
{

namespace qtquick
{

class SOFA_SOFAQTQUICKGUI_API Manipulator3D_Rotation : public Manipulator
{
    Q_OBJECT

public:
    explicit Manipulator3D_Rotation(QObject* parent = 0);
    ~Manipulator3D_Rotation();

public:
    Q_PROPERTY(QString axis READ axis WRITE setAxis NOTIFY axisChanged)

public:
    QString axis() const {return myAxis;}
    void setAxis(QString newAxis);

    bool displayMark() const    {return myDisplayMark;}
    float fromMarkAngle() const {return myFromMarkAngle;}
    float toMarkAngle() const   {return myToMarkAngle;}

signals:
    void axisChanged(QString newAxis);

public:
    Q_INVOKABLE void setMark(float fromAngle, float toAngle);
    Q_INVOKABLE void unsetMark();

public slots:
    virtual void draw(const Viewer& viewer) const;
    virtual void pick(const Viewer& viewer) const;

private:
    void internalDraw(const Viewer& viewer, bool isPicking = false) const;

private:
    QString myAxis;
    bool    myDisplayMark;
    float   myFromMarkAngle;
    float   myToMarkAngle;

};

}

}

#endif // MANIPULATOR3D_ROTATION_H
