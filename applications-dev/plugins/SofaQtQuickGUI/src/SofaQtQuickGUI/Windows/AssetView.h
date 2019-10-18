#pragma once

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Windows/CameraView.h>

#include <QtQuick/QQuickFramebufferObject>
#include <QVector3D>
#include <QVector4D>
#include <QImage>
#include <QColor>
#include <QList>

#include <sofa/helper/io/Image.h>


namespace sofa {
namespace qtquick {

/// \class Displays an asset in a QQuickFramebufferObject
/// \note Assets can be textures, meshes, prefabs, scripts etc.
class SOFA_SOFAQTQUICKGUI_API AssetView : public CameraView
{
    Q_OBJECT
public:
    explicit AssetView(QQuickItem* parent = nullptr);
    ~AssetView() override;
};

} // namespace qtquick
} // namespace sofa
