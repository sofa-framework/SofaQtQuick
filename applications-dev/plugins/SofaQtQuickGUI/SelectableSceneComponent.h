#ifndef SELECTABLE_SCENE_COMPONENT_H
#define SELECTABLE_SCENE_COMPONENT_H

#include "SofaQtQuickGUI.h"
#include "Selectable.h"
#include "SceneComponent.h"

namespace sofa
{

namespace qtquick
{

/// QtQuick wrapper for a selectable object representing a SceneComponent
class SOFA_SOFAQTQUICKGUI_API SelectableSceneComponent : public Selectable
{
    Q_OBJECT

public:
    SelectableSceneComponent(SceneComponent* sceneComponent);
    ~SelectableSceneComponent();

public:
    Q_PROPERTY(SceneComponent* sceneComponent READ sceneComponent)

public:
    SceneComponent* sceneComponent() const {return mySceneComponent;}

private:
    SceneComponent* mySceneComponent;

};

}

}

#endif // SELECTABLE_SCENE_COMPONENT_H
