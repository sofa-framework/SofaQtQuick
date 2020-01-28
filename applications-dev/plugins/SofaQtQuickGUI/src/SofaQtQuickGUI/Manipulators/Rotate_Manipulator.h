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
    Q_OBJECT
public:

    Rotate_Manipulator(QObject* parent = nullptr);

    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) override;
    virtual void mouseMoved(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mousePressed(const QPointF& mouse, SofaViewer* viewer) override;
    virtual void mouseReleased(const QPointF& mouse, SofaViewer* viewer) override;
    virtual int getIndices() const override;

    virtual QString getDisplayText() const override;

    static sofa::core::objectmodel::BaseData* GetData();
    bool getValue(QVector3D& direction, QQuaternion& orientation) const;
    void setValue(const QQuaternion&);



    Q_PROPERTY(bool local READ getLocal WRITE setLocal NOTIFY localChanged)
signals:
    void localChanged(bool);
private:
    bool getLocal();
    void setLocal(bool isLocal);
    bool m_isLocal;


private:
    void drawXAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& dt);
    void drawYAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& dt);
    void drawZAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& dt);
    void drawCamAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& dt);
    void drawTrackballAxis(const QVector3D& pos, sofa::core::visual::DrawToolGL& dt);


    float radius;
    float lineThickness;
    int resolution;
    float width;
    float height;

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
