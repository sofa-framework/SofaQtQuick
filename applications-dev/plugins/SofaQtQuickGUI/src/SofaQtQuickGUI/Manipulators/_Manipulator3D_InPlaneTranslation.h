#pragma once

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator.h>

#include <QObject>

namespace sofaqtquick
{

/// \class An object or world space manipulator allowing us to translate things along X, Y and / or the Z axis
class SOFA_SOFAQTQUICKGUI_API Manipulator3D_InPlaneTranslation : public Manipulator
{
    Q_OBJECT

public:
    explicit Manipulator3D_InPlaneTranslation(QObject* parent = nullptr);
    ~Manipulator3D_InPlaneTranslation();

public slots:
    virtual void draw(const SofaViewer& viewer) const;
    virtual void pick(const SofaViewer& viewer) const;

private:
    void internalDraw(const SofaViewer& viewer, bool isPicking = false) const;
};

}  // namespace sofaqtquick
