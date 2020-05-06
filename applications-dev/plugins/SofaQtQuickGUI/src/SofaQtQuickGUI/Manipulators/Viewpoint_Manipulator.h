#pragma once

#include "Manipulator.h"
#include <sofa/defaulttype/Vec.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Camera.h>


namespace sofaqtquick
{

class Viewpoint_Manipulator : public Manipulator
{
public:
    Viewpoint_Manipulator(QObject* parent = nullptr);

    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) override;
    virtual void mouseClicked(const QPointF& mouse, SofaViewer* viewer) override;
    virtual QString getDisplayText() const override;

    virtual int getIndices() const override;
};

}
