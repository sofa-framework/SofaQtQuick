#include <GL/glew.h>
#include "SelectableSceneComponent.h"

namespace sofa
{

namespace qtquick
{

SelectableSceneComponent::SelectableSceneComponent(SceneComponent* sceneComponent) : Selectable(sceneComponent),
    mySceneComponent(sceneComponent)
{

}

SelectableSceneComponent::~SelectableSceneComponent()
{

}

}

}
