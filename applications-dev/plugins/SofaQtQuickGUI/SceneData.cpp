#include <GL/glew.h>
#include "SceneData.h"
#include "SceneComponent.h"
#include "Scene.h"

#include <QString>

namespace sofa
{

namespace qtquick
{

using namespace sofa::core::objectmodel;

SceneData::SceneData(const SceneComponent* sceneComponent, const sofa::core::objectmodel::BaseData* data) : QObject(),
    myScene(sceneComponent->scene()),
    myBase(sceneComponent->base()),
    myData(data)
{

}

SceneData::SceneData(const Scene* scene, const sofa::core::objectmodel::Base* base, const sofa::core::objectmodel::BaseData* data) : QObject(),
    myScene(scene),
    myBase(base),
    myData(data)
{

}

SceneData::SceneData(const SceneData& sceneData) :
    myScene(sceneData.myScene),
    myBase(),
    myData(sceneData.data())
{
    // if the data exists, its base is valid
    if(myData)
        myBase = sceneData.myBase;
}

QVariantMap SceneData::object() const
{
    const BaseData* data = SceneData::data();
    if(data)
        return Scene::dataObject(data);

    return QVariantMap();
}

QVariant SceneData::value()
{
    BaseData* data = SceneData::data();
    if(data)
        return Scene::dataValue(data);

    return QVariant();
}

bool SceneData::setValue(const QVariant& value)
{
    BaseData* data = SceneData::data();
    if(data)
        return Scene::setDataValue(data, value);

    return false;
}

bool SceneData::setLink(const QString& path)
{
    BaseData* data = SceneData::data();
    if(data)
    {
        std::streambuf* backup(std::cerr.rdbuf());

        std::ostringstream stream;
        std::cerr.rdbuf(stream.rdbuf());
        bool status = Scene::setDataLink(data, path);
        std::cerr.rdbuf(backup);

        return status;
    }

    return false;
}

BaseData* SceneData::data()
{
    return const_cast<BaseData*>(static_cast<const SceneData*>(this)->data());
}

const BaseData* SceneData::data() const
{
    // check if the base still exists hence if the data is still valid
    const Base* base = 0;
    if(myScene && myBase)
        if(myScene->myBases.contains(myBase))
            base = myBase;

    myBase = base;
    if(!myBase)
        myData = 0;

    return myData;
}

}

}
