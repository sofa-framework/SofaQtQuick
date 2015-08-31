#ifndef SCENE_COMPONENT_H
#define SCENE_COMPONENT_H

#include "SofaQtQuickGUI.h"
#include "SceneData.h"
#include <sofa/simulation/common/Simulation.h>

namespace sofa
{

namespace qtquick
{

class Scene;

/// QtQuick wrapper for a Sofa component (i.e baseObject / baseNode), allowing us to share a component in a QML context
class SOFA_SOFAQTQUICKGUI_API SceneComponent : public QObject
{
    Q_OBJECT

    friend class SceneData;

public:
    SceneComponent(const Scene* scene, const sofa::core::objectmodel::Base* base);
    SceneComponent(const SceneComponent& sceneComponent);

public:
    Q_PROPERTY(QString name READ name)

public:
    QString name() const;

public:
    Q_INVOKABLE bool isSame(SceneComponent* sceneComponent);
    Q_INVOKABLE sofa::qtquick::SceneData* getComponentData(const QString& name) const;

public:
    sofa::core::objectmodel::Base* base();
    const sofa::core::objectmodel::Base* base() const;

    const Scene* scene() const;

private:
    const Scene*                                    myScene;
    mutable const sofa::core::objectmodel::Base*    myBase;
};

}

}

#endif // SCENE_COMPONENT_H
