#ifndef MANIPULATOR2D_ROTATION_H
#define MANIPULATOR2D_ROTATION_H

#include "SofaQtQuickGUI.h"
#include "Manipulator.h"

#include <QObject>
#include <QVector3D>

namespace sofa
{

namespace qtquick
{

class SOFA_SOFAQTQUICKGUI_API Manipulator2D_Rotation : public Manipulator
{
    Q_OBJECT

public:
    explicit Manipulator2D_Rotation(QObject* parent = 0);
    ~Manipulator2D_Rotation();

public:
    Q_INVOKABLE void setMark(float fromAngle, float toAngle);
    Q_INVOKABLE void unsetMark();

public slots:
    virtual void draw(const Viewer& viewer) const;

private:
    bool  myDisplayMark;
    float myFromMarkAngle;
    float myToMarkAngle;

};

}

}

#endif // MANIPULATOR2D_ROTATION_H
