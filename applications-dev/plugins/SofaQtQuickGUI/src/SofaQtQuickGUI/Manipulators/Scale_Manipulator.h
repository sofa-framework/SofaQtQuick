#pragma once

#include "Manipulator.h"
#include <sofa/defaulttype/Vec.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Camera.h>

namespace sofaqtquick
{

class Scale_Manipulator : public Manipulator
{
    Q_OBJECT
public:
    Scale_Manipulator(QObject* parent = nullptr);

    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) override;
    virtual void mouseMoved(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mousePressed(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mouseReleased(const QPointF& mouse, SofaViewer* viewer) override;
    virtual QString getDisplayText() const override;

    virtual int getIndices() const override;
    void drawXYPlane() const;

    static sofa::core::objectmodel::BaseData* GetData();
    
    Q_PROPERTY(bool uniform READ getUniform WRITE setUniform NOTIFY uniformChanged)
signals:
    void uniformChanged(bool);
private:
    bool getUniform() { return m_isUniform; }
    void setUniform(bool isUniform) { m_isUniform = isUniform; }
    bool m_isUniform;

private:
    struct s_color {
        sofa::defaulttype::Vec4f highlight;
        sofa::defaulttype::Vec4f light;
        sofa::defaulttype::Vec4f color;
        sofa::defaulttype::Vec3f primitive;
    };

    void drawFace(const std::vector<sofa::defaulttype::Vec3d>& q, const s_color color, sofa::core::visual::DrawTool& dt, int idx, bool dir = true);

    float radius;
    float lineThickness;
    float crossSize;
    double arrowLength;
    double squareWidth;

    bool _pickIndex;
    bindings::SofaBase* obj;
    sofa::Data<sofa::defaulttype::Vec3d>* data;
    Camera* cam;


    s_color blue;
    s_color green;
    s_color red;
    s_color white;

    sofa::defaulttype::Vec4f black;
    sofa::defaulttype::Vec4f yellow;

    QVector3D shift;
    double initialAxisLength;
    bool active {false};
};

}  // namespace sofaqtquick
