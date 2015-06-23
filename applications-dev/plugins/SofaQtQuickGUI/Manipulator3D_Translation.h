#ifndef MANIPULATOR3D_TRANSLATION_H
#define MANIPULATOR3D_TRANSLATION_H

#include "SofaQtQuickGUI.h"
#include "Manipulator.h"

#include <QObject>
#include <QVector3D>

namespace sofa
{

namespace qtquick
{

class SOFA_SOFAQTQUICKGUI_API Manipulator3D_Translation : public Manipulator
{
    Q_OBJECT

public:
    explicit Manipulator3D_Translation(QObject* parent = 0);
    ~Manipulator3D_Translation();

public:
    Q_PROPERTY(QString axis READ axis WRITE setAxis NOTIFY axisChanged)

public:
    QString axis() const {return myAxis;}
    void setAxis(QString newAxis);

signals:
    void axisChanged(QString newAxis);

public slots:
    virtual void draw(const Viewer& viewer) const;

private:
    QString myAxis;

};

}

}

#endif // MANIPULATOR3D_TRANSLATION_H
