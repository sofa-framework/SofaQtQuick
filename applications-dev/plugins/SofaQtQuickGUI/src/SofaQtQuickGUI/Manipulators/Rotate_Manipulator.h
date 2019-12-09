#pragma once

#include "Manipulator.h"
#include <sofa/defaulttype/Vec.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Camera.h>

namespace sofaqtquick
{

class Rotate_Manipulator : public Manipulator
{
public:
    Rotate_Manipulator(QObject* parent = nullptr);

    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) override;
    virtual void mouseMoved(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mousePressed(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mouseReleased(const QPointF& mouse, SofaViewer* viewer) override;
    virtual int getIndices() const override;
    
    static sofa::Data<sofa::defaulttype::Vec3d>* getData();

private:
    void drawXAxis(const sofa::defaulttype::Vec3d& pos);
    void drawYAxis(const sofa::defaulttype::Vec3d& pos);
    void drawZAxis(const sofa::defaulttype::Vec3d& pos);
    void drawCamAxis(const sofa::defaulttype::Vec3d& pos);

    void setMark(double from, double to);
    void unsetMark();

    float radius;
    float lineThickness;
    int resolution;
    float width;
    float height;

    sofa::core::visual::DrawToolGL drawtools;
    bindings::SofaBase* obj;
    sofa::Data<sofa::defaulttype::Vec3d>* data;
    Camera* cam;

    sofa::defaulttype::Vec4f highlightred;
    sofa::defaulttype::Vec4f highlightgreen;
    sofa::defaulttype::Vec4f highlightblue;
    sofa::defaulttype::Vec4f highlightwhite;

    sofa::defaulttype::Vec4f lightred;
    sofa::defaulttype::Vec4f lightgreen;
    sofa::defaulttype::Vec4f lightblue;

    sofa::defaulttype::Vec4f red;
    sofa::defaulttype::Vec4f green;
    sofa::defaulttype::Vec4f blue;
    sofa::defaulttype::Vec4f white;
    sofa::defaulttype::Vec4f lightwhite;
    sofa::defaulttype::Vec4f black;
    sofa::defaulttype::Vec4f yellow;

    bool drawMark {false};
    double _from, _to, _startAngle {0};
    sofa::defaulttype::Quaternion startOrientation;
    QVector3D mX, mY, mZ, mCam;
};

}  // namespace sofaqtquick
