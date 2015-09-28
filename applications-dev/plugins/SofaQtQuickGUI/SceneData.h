#ifndef SCENE_DATA_H
#define SCENE_DATA_H

#include "SofaQtQuickGUI.h"

#include <sofa/simulation/common/Simulation.h>
#include <QVariantMap>

class QTimer;
class QOpenGLShaderProgram;

namespace sofa
{

namespace qtquick
{

class Scene;
class SceneComponent;

/// QtQuick wrapper for a Sofa base data, allowing us to share a component data in a QML context
class SOFA_SOFAQTQUICKGUI_API SceneData : public QObject
{
    Q_OBJECT

public:
    SceneData(const SceneComponent* sceneComponent, const sofa::core::objectmodel::BaseData* data);
    SceneData(const Scene* scene, const sofa::core::objectmodel::Base* base, const sofa::core::objectmodel::BaseData* data);
//    SceneData(const SceneData& sceneData);

    Q_INVOKABLE QVariantMap object() const;

    Q_INVOKABLE QVariant value();
    Q_INVOKABLE bool setValue(const QVariant& value);
    Q_INVOKABLE bool setLink(const QString& path);

    sofa::core::objectmodel::BaseData* data();
    const sofa::core::objectmodel::BaseData* data() const;

private:
    const Scene*                                        myScene;
    mutable const sofa::core::objectmodel::Base*        myBase;
    mutable const sofa::core::objectmodel::BaseData*    myData;

};

}

}

#endif // SCENE_DATA_H
