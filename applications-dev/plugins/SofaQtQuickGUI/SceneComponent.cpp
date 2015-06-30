#include <GL/glew.h>
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

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::component::visualmodel;
using namespace sofa::simulation;

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
