#include <GL/glew.h>
#include "SceneComponent.h"
#include "Scene.h"

namespace sofa
{

namespace qtquick
{

using namespace sofa::core::objectmodel;

SceneComponent::SceneComponent(const Scene* scene, const sofa::core::objectmodel::Base* base) : QObject(),
    myScene(scene),
    myBase(base)
{

}

SceneComponent::SceneComponent(const SceneComponent& sceneComponent) :
    myScene(sceneComponent.scene()),
    myBase(sceneComponent.base())
{

}

QString SceneComponent::name() const
{
    const Base* base = SceneComponent::base();
    if(!base)
        return QString("Invalid SceneComponent");

    return QString::fromStdString(base->getName());
}

Base* SceneComponent::base()
{
    return const_cast<Base*>(static_cast<const SceneComponent*>(this)->base());
}

const Base* SceneComponent::base() const
{
    // check object existence
    if(myScene && myBase)
        if(myScene->myBases.contains(myBase))
            return myBase;

    myBase = 0;
    return myBase;
}

const Scene* SceneComponent::scene() const
{
    return myScene;
}

}

}
