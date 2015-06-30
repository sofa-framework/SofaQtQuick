#include <GL/glew.h>
#include "SceneData.h"
#include "SceneComponent.h"
#include "Scene.h"

#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/core/objectmodel/GUIEvent.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/PluginManager.h>
#include <sofa/simulation/common/xml/initXml.h>
#include <sofa/simulation/graph/DAGSimulation.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <sofa/helper/system/glut.h>
#include <SofaPython/SceneLoaderPY.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaComponentMain/init.h>

#include <array>
#include <sstream>
#include <qqml.h>
#include <QtCore/QCoreApplication>
#include <QVector3D>
#include <QStack>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QUrl>
#include <QThread>
#include <QSequentialIterable>
#include <QJSValue>
#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QOpenGLShaderProgram>

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::component::visualmodel;
using namespace sofa::simulation;

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
