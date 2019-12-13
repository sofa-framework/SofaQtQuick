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

    virtual QString getDisplayText() const override;

    static sofa::core::objectmodel::BaseData* getData();

private:
    void drawXAxis(const QVector3D& pos);
    void drawYAxis(const QVector3D& pos);
    void drawZAxis(const QVector3D& pos);
    void drawCamAxis(const QVector3D& pos);
    void drawTrackballAxis(const QVector3D& pos);

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

    float _from, _to;
    QQuaternion startOrientation;
    QVector3D startDirection;
    QVector3D mX, mY, mZ, mCam;
    bool drawMark {false};
};

}  // namespace sofaqtquick
