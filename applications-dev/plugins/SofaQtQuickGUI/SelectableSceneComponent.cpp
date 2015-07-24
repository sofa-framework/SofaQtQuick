#include <GL/glew.h>
#include "SelectableSceneComponent.h"

namespace sofa
{

namespace qtquick
{

SelectableSceneComponent::SelectableSceneComponent(const SceneComponent& sceneComponent) : Selectable(),
    mySceneComponent(new SceneComponent(sceneComponent))
{

}

SelectableSceneComponent::~SelectableSceneComponent()
{
    delete mySceneComponent;
}

}

}
