#pragma once

#include "Manipulator.h"
#include <sofa/defaulttype/Vec.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Camera.h>

namespace sofaqtquick
{


/// Multi-selection particle manipulator:
/// using the shift key, multiple particles (of a same object) can be selected for translation.
/// The manipulator's gizmo will be displayed at the barycentric coordinates of the selected point cloud
class TranslateParticle_Manipulator : public Manipulator
{
public:
    TranslateParticle_Manipulator(QObject* parent = nullptr);

    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) override;
    virtual void mouseMoved(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mousePressed(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mouseReleased(const QPointF& mouse, SofaViewer* viewer) override;
    virtual QString getDisplayText() const override;

    virtual int getIndices() const override;
    void drawXYPlane() const;

    static sofa::core::objectmodel::BaseData* getData();
    
private:
    void drawXArrow(const sofa::defaulttype::Vec3d& pos);
    void drawYArrow(const sofa::defaulttype::Vec3d& pos);
    void drawZArrow(const sofa::defaulttype::Vec3d& pos);
    void drawXYPlane(const sofa::defaulttype::Vec3d& pos);
    void drawYZPlane(const sofa::defaulttype::Vec3d& pos);
    void drawZXPlane(const sofa::defaulttype::Vec3d& pos);
    void drawCamPlane(const sofa::defaulttype::Vec3d& pos, bool isPicking);

    float radius;
    float lineThickness;
    float crossSize;
    double arrowLength;
    double squareWidth;

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
    sofa::defaulttype::Vec4f lightwhite;

    sofa::defaulttype::Vec4f red;
    sofa::defaulttype::Vec4f green;
    sofa::defaulttype::Vec4f blue;
    sofa::defaulttype::Vec4f white;
    sofa::defaulttype::Vec4f black;
    sofa::defaulttype::Vec4f yellow;

    QVector3D shift;
    bool active {false};
};

}  // namespace sofaqtquick
