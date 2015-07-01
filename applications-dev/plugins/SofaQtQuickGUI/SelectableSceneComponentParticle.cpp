#include <GL/glew.h>
#include "SelectableSceneComponentParticle.h"

namespace sofa
{

namespace qtquick
{

SelectableSceneComponentParticle::SelectableSceneComponentParticle(SceneComponent* sceneComponent, int particleIndex) : SelectableSceneComponent(sceneComponent),
    myParticleIndex(particleIndex)
{

}

SelectableSceneComponentParticle::~SelectableSceneComponentParticle()
{

}

}

}
