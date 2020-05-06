#pragma once

#include "Manipulator.h"
#include <sofa/defaulttype/Vec.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Camera.h>

namespace sofaqtquick
{

class Translate_Manipulator : public Manipulator
{
public:
    Translate_Manipulator(QObject* parent = nullptr);

    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) override;
    virtual void mouseMoved(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mousePressed(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mouseReleased(const QPointF& mouse, SofaViewer* viewer) override;
    virtual QString getDisplayText() const override;

    virtual int getIndices() const override;
    void drawXYPlane() const;

    static sofa::core::objectmodel::BaseData* GetData();
    static bool GetValue(QVector3D& value, bool editMode, int pIndex);
    bool getValue(QVector3D&) const;
    void setValue(const QVector3D&);

private:
    void drawXArrow(const sofa::defaulttype::Vec3d& pos, sofa::core::visual::DrawToolGL&);
    void drawYArrow(const sofa::defaulttype::Vec3d& pos, sofa::core::visual::DrawToolGL&);
    void drawZArrow(const sofa::defaulttype::Vec3d& pos, sofa::core::visual::DrawToolGL&);
    void drawXYPlane(const sofa::defaulttype::Vec3d& pos, sofa::core::visual::DrawToolGL&);
    void drawYZPlane(const sofa::defaulttype::Vec3d& pos, sofa::core::visual::DrawToolGL&);
    void drawZXPlane(const sofa::defaulttype::Vec3d& pos, sofa::core::visual::DrawToolGL&);
    void drawCamPlane(const sofa::defaulttype::Vec3d& pos, bool isPicking, sofa::core::visual::DrawToolGL&);

    float radius;
    float lineThickness;
    float crossSize;
    double arrowLength;
    double squareWidth;

    bindings::SofaBase* obj;
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
