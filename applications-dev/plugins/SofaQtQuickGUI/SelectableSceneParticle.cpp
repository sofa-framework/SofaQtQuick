#include <GL/glew.h>
#include "SelectableSceneParticle.h"

namespace sofa
{

namespace qtquick
{

SelectableSceneParticle::SelectableSceneParticle(const SceneComponent& sceneComponent, int particleIndex) : SelectableSceneComponent(sceneComponent),
    myParticleIndex(particleIndex)
{

}

SelectableSceneParticle::~SelectableSceneParticle()
{

}

}

}
