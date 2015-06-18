#ifndef MANIPULATOR2D_TRANSLATION_H
#define MANIPULATOR2D_TRANSLATION_H

#include "SofaQtQuickGUI.h"
#include "Manipulator.h"

#include <QObject>
#include <QVector3D>

namespace sofa
{

namespace qtquick
{

class SOFA_SOFAQTQUICKGUI_API Manipulator2D_Translation : public Manipulator
{
    Q_OBJECT

public:
    explicit Manipulator2D_Translation(QObject* parent = 0);
    ~Manipulator2D_Translation();

public:
    Q_PROPERTY(bool xAxis READ xAxis WRITE setXAxis NOTIFY xAxisChanged)
    Q_PROPERTY(bool yAxis READ yAxis WRITE setYAxis NOTIFY yAxisChanged)

public:
    bool xAxis() const {return myXAxis;}
    void setXAxis(bool newXAxis);

    bool yAxis() const {return myYAxis;}
    void setYAxis(bool newYAxis);

signals:
    void xAxisChanged(bool newXAxis);
    void yAxisChanged(bool newYAxis);

public slots:
    virtual void draw(const Viewer& viewer) const;

private:
    bool myXAxis;
    bool myYAxis;

};

}

}

#endif // MANIPULATOR2D_TRANSLATION_H
