/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SofaScene.h"
#include "SofaViewer.h"
#include "SelectableManipulator.h"

#include <sofa/helper/OptionsGroup.h>
using sofa::helper::OptionsGroup ;

#include <sofa/core/objectmodel/DataFileName.h>
using sofa::core::objectmodel::DataFileName ;

#include <sofa/helper/types/RGBAColor.h>
using sofa::helper::types::RGBAColor ;

#include <sofa/helper/system/FileSystem.h>
using sofa::helper::system::FileSystem ;

#include <sofa/helper/AdvancedTimer.h>

#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/Tag.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/core/objectmodel/GUIEvent.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/PluginManager.h>
#include <sofa/helper/logging/Messaging.h>
#include <sofa/helper/cast.h>
#include <sofa/simulation/UpdateBoundingBoxVisitor.h>
#include <SofaSimulationGraph/graph.h>
#include <SofaSimulationGraph/DAGSimulation.h>
#include <SofaSimulationGraph/init.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <sofa/core/visual/VisualModel.h>
#include <SofaPython/SceneLoaderPY.h>
#include <SofaPython/PythonScriptController.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaMeshCollision/TriangleModel.h>
#include <SofaComponentCommon/initComponentCommon.h>
#include <SofaComponentBase/initComponentBase.h>
#include <SofaComponentGeneral/initComponentGeneral.h>
#include <SofaComponentAdvanced/initComponentAdvanced.h>
#include <SofaComponentMisc/initComponentMisc.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/simulation/MechanicalVisitor.h>
#include <sofa/simulation/DeleteVisitor.h>

#include <array>
#include <sstream>
#include <qqml.h>
#include <QtCore/QCoreApplication>
#include <QVector3D>
#include <QStack>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QThread>
#include <QSequentialIterable>
#include <QJSValue>
#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QRunnable>
#include <QGuiApplication>
#include <QOffscreenSurface>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core;
using namespace sofa::core::objectmodel;
using namespace sofa::core::visual;
using namespace sofa::component::visualmodel;
using namespace sofa::component::collision;
using namespace sofa::component::controller;
using namespace sofa::simulation;

typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3Types> MechanicalObject3;

SofaScene::SofaScene(QObject *parent) : QObject(parent), MutationListener(),
    myStatus(Status::Null),
    mySource(),
    mySourceQML(),
    myPath(),
    myPathQML(),
    myVisualDirty(false),
    myDt(0.04),
    myAnimate(false),
    myExtraParams(),
    myDefaultAnimate(false),
    myAsynchronous(true),
    myPyQtForceSynchronous(true),
    mySofaSimulation(nullptr),
    myStepTimer(new QTimer(this)),
    myBases(),
    myManipulators(),
    mySelectedManipulator(nullptr),
    mySelectedComponent(nullptr),
    myHighlightShaderProgram(nullptr),
    myPickingShaderProgram(nullptr),
    myPickingFBO(nullptr)
{

    sofa::simulation::graph::init();
    sofa::component::initComponentBase();
    sofa::component::initComponentCommon();
    sofa::component::initComponentGeneral();
    sofa::component::initComponentAdvanced();
    sofa::component::initComponentMisc();

    sofa::core::ExecParams::defaultInstance()->setAspectID(0);
    sofa::core::ObjectFactory::ClassEntry::SPtr classVisualModel;
    sofa::core::ObjectFactory::AddAlias("VisualModel", "OglModel", true, &classVisualModel);

    myStepTimer->setInterval(0);
    mySofaSimulation = sofa::simulation::graph::getSimulation();

    // plugins
    QVector<QString> plugins;
    plugins.append("SofaPython");

    for(const QString& plugin : plugins)
    {
        std::string s = plugin.toStdString();
        sofa::helper::system::PluginManager::getInstance().loadPlugin(s);
    }

    //Autoload
    std::string configPluginPath = sofa::helper::system::PluginRepository.getFirstPath() + "/" + TOSTRING(CONFIG_PLUGIN_FILENAME);
    std::string defaultConfigPluginPath = sofa::helper::system::PluginRepository.getFirstPath() + "/" + TOSTRING(DEFAULT_CONFIG_PLUGIN_FILENAME);
    if (sofa::helper::system::DataRepository.findFile(configPluginPath))
    {
        msg_info("qtSofaQuick") << "Loading automatically custom plugin list from " << configPluginPath;
        sofa::helper::system::PluginManager::getInstance().readFromIniFile(configPluginPath);
    }
    else if (sofa::helper::system::DataRepository.findFile(defaultConfigPluginPath))
    {
        msg_info("qtSofaQuick") << "Loading automatically default plugin list from " << defaultConfigPluginPath;
        sofa::helper::system::PluginManager::getInstance().readFromIniFile(defaultConfigPluginPath);
    }
    else
        msg_info("qtSofaQuick") << "No plugin will be automatically loaded" << msgendl
        << "- No custom list found at " << configPluginPath << msgendl
        << "- No default list found at " << defaultConfigPluginPath;


    sofa::helper::system::PluginManager::getInstance().init();

    // connections
    connect(this, &SofaScene::sourceChanged, this, &SofaScene::open);
    connect(this, &SofaScene::animateChanged, myStepTimer, [&](bool newAnimate) {newAnimate ? myStepTimer->start() : myStepTimer->stop();});
    connect(this, &SofaScene::statusChanged, this, &SofaScene::handleStatusChange);
    connect(this, &SofaScene::aboutToUnload, this, [&]() {myBases.clear();});

    connect(myStepTimer, &QTimer::timeout, this, &SofaScene::step);
}

SofaScene::~SofaScene()
{
    setSource(QUrl());

    if(mySofaSimulation == sofa::simulation::getSimulation())
        sofa::simulation::setSimulation(0);

    sofa::simulation::graph::cleanup();
}

bool LoaderProcess(SofaScene* sofaScene, QOffscreenSurface* offscreenSurface)
{
    if(!sofaScene || !sofaScene->sofaSimulation() || sofaScene->path().isEmpty())
        return false;

    if(!QOpenGLContext::currentContext())
    {
        // create an opengl context
        QOpenGLContext* openglContext = new QOpenGLContext();

        // share its resources with the global context
        QOpenGLContext* sharedOpenglContext = QOpenGLContext::globalShareContext();
        if(sharedOpenglContext)
            openglContext->setShareContext(sharedOpenglContext);

        if(!openglContext->create())
            qFatal("Cannot create an OpenGL Context needed to init SofaScene");

        if(!offscreenSurface->isValid())
            qFatal("Cannot create an OpenGL Surface needed to init SofaScene");

        openglContext->makeCurrent(offscreenSurface);
    }

    //qDebug() << "OpenGL" << QOpenGLContext::currentContext()->format().majorVersion() << "." << QOpenGLContext::currentContext()->format().minorVersion();

    GLenum err = glewInit();
    if(0 != err)
        msg_error("SofaQtQuickGUI") << "GLEW Initialization failed with error code:" << err;

    // init the highlight shader program
    if(!sofaScene->myHighlightShaderProgram)
    {
        sofaScene->myHighlightShaderProgram = new QOpenGLShaderProgram();

        sofaScene->myHighlightShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            "void main(void)\n"
            "{\n"
            "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}");
        sofaScene->myHighlightShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            "void main(void)\n"
            "{\n"
            "   gl_FragColor = vec4(0.75, 0.5, 0.0, 1.0);\n"
            "}");

        sofaScene->myHighlightShaderProgram->link();

        sofaScene->myHighlightShaderProgram->moveToThread(sofaScene->thread());
        sofaScene->myHighlightShaderProgram->setParent(sofaScene);
    }

    // init the picking shader program
    if(!sofaScene->myPickingShaderProgram)
    {
        sofaScene->myPickingShaderProgram = new QOpenGLShaderProgram();
        sofaScene->myPickingShaderProgram->create();
        sofaScene->myPickingShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            "void main(void)\n"
            "{\n"
            "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}");
        sofaScene->myPickingShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            "uniform highp vec4 index;\n"
            "void main(void)\n"
            "{\n"
            "   gl_FragColor = index;\n"
            "}");
        sofaScene->myPickingShaderProgram->link();

        sofaScene->myPickingShaderProgram->moveToThread(sofaScene->thread());
        sofaScene->myPickingShaderProgram->setParent(sofaScene);
    }

    // prepare the sofa visual params
    sofa::core::visual::VisualParams* visualParams = sofa::core::visual::VisualParams::defaultInstance();
    if(visualParams)
    {
        if(!visualParams->drawTool())
        {
            visualParams->drawTool() = new sofa::core::visual::DrawToolGL();
            visualParams->setSupported(sofa::core::visual::API_OpenGL);
        }

        visualParams->displayFlags().setShowVisualModels(true);
    }

    sofaScene->sofaRootNode() = sofaScene->sofaSimulation()->load(sofaScene->path().toLatin1().constData(), sofaScene->extraParams());
    if( sofaScene->sofaRootNode() )
    {
        sofaScene->sofaSimulation()->init(sofaScene->sofaRootNode().get());
        sofaScene->sofaSimulation()->initTextures(sofaScene->sofaRootNode().get());

        glFinish();

        sofaScene->addChild(0, sofaScene->sofaRootNode().get());

        if(sofaScene->sofaRootNode()->getAnimate() || sofaScene->defaultAnimate())
            sofaScene->setAnimate(true);

        sofaScene->setStatus(SofaScene::Status::Ready);

        if(!sofaScene->pathQML().isEmpty())
            sofaScene->setSourceQML(QUrl::fromLocalFile(sofaScene->pathQML()));

        return true;
    }
    else
    {
        sofaScene->setStatus(SofaScene::Status::Error);
        return false;
    }

    sofaScene->setStatus(SofaScene::Status::Error);

    return false;
}

class LoaderThread : public QThread
{
public:
    LoaderThread(SofaScene* sofaScene, QOffscreenSurface* offscreenSurface) :
        mySofaScene(sofaScene),
        myOffscreenSurface(offscreenSurface),
        myIsLoaded(false)
    {

    }

    void run()
    {
        myIsLoaded = LoaderProcess(mySofaScene, myOffscreenSurface);
    }

    bool isLoaded() const			{return myIsLoaded;}

private:
    SofaScene*                      mySofaScene;
    QOffscreenSurface*              myOffscreenSurface;
    bool							myIsLoaded;

};

class WaitTillSwapWorker : public QRunnable
{
public:
    WaitTillSwapWorker(bool& finished) :
        myFinished(finished)
    {

    }

    void run()
    {
        myFinished = true;
    }

private:
    bool&   myFinished;

};

void SofaScene::open()
{
// clear the qml interface

    setPathQML("");
    setSourceQML(QUrl());

// return now if a scene is already loading

    if(Status::Loading == myStatus)
        return;

// reset properties

    setAnimate(false);

    setSelectedComponent(nullptr);
    setSelectedManipulator(nullptr);
    myManipulators.clear();

    if(mySofaRootNode)
    {
        setStatus(Status::Unloading);

        aboutToUnload();

        mySofaSimulation->unload(mySofaRootNode);
    }

    if(mySource.isEmpty())
    {
        setStatus(Status::Null);
        setPath("");
        return;
    }

    QString finalFilename = mySource.path();
    if(mySource.isLocalFile())
        finalFilename = mySource.toLocalFile();

    if(finalFilename.isEmpty())
    {
        setStatus(Status::Error);
        setPath("");
        return;
    }

    std::string filepath = finalFilename.toLatin1().constData();
    if(!sofa::helper::system::DataRepository.findFile(filepath))
    {
        setStatus(Status::Error);
        setPath("");
        return;
    }

    finalFilename = filepath.c_str();
    finalFilename.replace("\\", "/");

    setPath(finalFilename);
    setStatus(Status::Loading);

// does scene contain PyQt ? if so, load it synchronously to avoid instantiation of a QApplication outside of the main thread

    bool currentAsynchronous = myAsynchronous;
    if(myPyQtForceSynchronous && currentAsynchronous)
    {
        QFile file(QString::fromStdString(filepath));
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            QString content = in.readAll();
            if(-1 != content.indexOf("PyQt", 0, Qt::CaseInsensitive)) {
                currentAsynchronous = false;
                msg_warning("SofaQtQuickGUI")  << "This scene seems to contain PyQt and will be loaded synchronously";
            }
        }
    }

// set the qml interface

    QString qmlFilepath = finalFilename;
    int extensionIndex = qmlFilepath.lastIndexOf('.');
    if(-1 != extensionIndex)
        qmlFilepath = qmlFilepath.left(extensionIndex);

    qmlFilepath += ".qml";

    std::string finalQmlFilepath = qmlFilepath.toLatin1().constData();
    if(!sofa::helper::system::DataRepository.findFile(finalQmlFilepath, "", nullptr))
    {
        // TODO: for backward compatibility only, we try to load the file myscene.ext.qml instead of juste myscene.qml
        finalQmlFilepath = (finalFilename + ".qml").toLatin1().constData();
        if(!sofa::helper::system::DataRepository.findFile(finalQmlFilepath, "", nullptr))
            finalQmlFilepath.clear();
        else
            msg_deprecated("SofaQtQuickGUI") << "The extension format of your scene qml interface is deprecated, use directly ***.qml instead of ***.py.qml";
    }
    setPathQML(QString::fromStdString(finalQmlFilepath));

// python header

    QString finalHeader;
    QFile baseHeaderFile(":/python/BaseHeader.py");
    if(!baseHeaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        msg_error("SofaQtQuickGUI") << "base header not found";
    }
    else
    {
        finalHeader += QTextStream(&baseHeaderFile).readAll();
    }

    finalHeader += myHeader;

    SceneLoaderPY::setHeader(finalHeader.toStdString());

// load the requested scene synchronously / asynchronously

    QOffscreenSurface* offScreenSurface = new QOffscreenSurface();
    offScreenSurface->create();

    if(currentAsynchronous)
    {
        LoaderThread* loaderThread = new LoaderThread(this, offScreenSurface);

        connect(loaderThread, &QThread::finished, this, [this, loaderThread, offScreenSurface]() {
            if(!loaderThread->isLoaded())
                setStatus(Status::Error);
            else
            {
                setDt(mySofaRootNode->getDt());
            }

            loaderThread->deleteLater();
            offScreenSurface->deleteLater();
        });

        loaderThread->start();

        /*
        QWindowList windows = qGuiApp->topLevelWindows();
        for(QWindow* window : windows)
        {
            QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(window);
            if(quickWindow)
            {
                window()->scheduleRenderJob(loaderThread, QQuickWindow::AfterSynchronizingStage);
                window()->update();
                break;
            }
        }

        // TODO: add a timeout
        while(!finished)
            qApp->processEvents(QEventLoop::WaitForMoreEvents);
        */
    }
    else
    {
        if(!LoaderProcess(this, offScreenSurface))
            setStatus(Status::Error);
        else
        {
            setDt(mySofaRootNode->getDt());
            offScreenSurface->deleteLater();
        }
    }
}

void SofaScene::handleStatusChange(SofaScene::Status newStatus)
{
    switch(newStatus)
    {
    case Status::Null:
        break;
    case Status::Ready:
        loaded();
        break;
    case Status::Loading:
        break;
    case Status::Unloading:
        break;
    case Status::Error:
        break;
    default:
        qCritical() << "SofaScene status unknown";
        break;
    }
}

void SofaScene::setStatus(Status newStatus)
{
    if(newStatus == myStatus)
        return;

    myStatus = newStatus;

    statusChanged(newStatus);
}

void SofaScene::setHeader(const QString& newHeader)
{
    if(newHeader == myHeader || Status::Loading == myStatus)
        return;

    myHeader = newHeader;

    headerChanged(newHeader);
}

void SofaScene::setSource(const QUrl& newSource)
{
    if(newSource == mySource || Status::Loading == myStatus)
        return;

    mySource = newSource;

    sourceChanged(newSource);
}

void SofaScene::setSourceQML(const QUrl& newSourceQML)
{
    if(newSourceQML == mySourceQML)
        return;

    mySourceQML = newSourceQML;

    sourceQMLChanged(newSourceQML);
}

void SofaScene::setPath(const QString& newPath)
{
    if(newPath == myPath)
        return;

    myPath = newPath;

    pathChanged(newPath);
}

void SofaScene::setPathQML(const QString& newPathQML)
{
    if(newPathQML == myPathQML)
        return;

    myPathQML = newPathQML;

    pathQMLChanged(newPathQML);
}

void SofaScene::setExtraParams(const std::vector<std::string> &newExtraParams)
{
    myExtraParams = newExtraParams;
}

void SofaScene::setDt(double newDt)
{
    if(newDt == myDt)
        return;

    myDt = newDt;

    dtChanged(newDt);
}

void SofaScene::setAnimate(bool newAnimate)
{
    if(newAnimate == myAnimate)
        return;

    myAnimate = newAnimate;
    if(sofaSimulation() && sofaRootNode())
        sofaRootNode()->setAnimate(myAnimate);

    animateChanged(newAnimate);
}

void SofaScene::setDefaultAnimate(bool newDefaultAnimate)
{
    if(newDefaultAnimate == myDefaultAnimate)
        return;

    myDefaultAnimate = newDefaultAnimate;

    defaultAnimateChanged(newDefaultAnimate);
}

void SofaScene::setAsynchronous(bool newAsynchronous)
{
    if(newAsynchronous == myAsynchronous)
        return;

    myAsynchronous = newAsynchronous;

    asynchronousChanged(newAsynchronous);
}

void SofaScene::setPyQtForceSynchronous(bool newPyQtForceSynchronous)
{
    if(newPyQtForceSynchronous == myPyQtForceSynchronous)
        return;

    myPyQtForceSynchronous = newPyQtForceSynchronous;

    pyQtForceSynchronousChanged(newPyQtForceSynchronous);
}

void SofaScene::setSelectedComponent(sofa::qtquick::SofaComponent* newSelectedComponent)
{
    if(newSelectedComponent == mySelectedComponent)
        return;

    delete mySelectedComponent;
    mySelectedComponent = nullptr;

    if(newSelectedComponent)
        mySelectedComponent = new SofaComponent(*newSelectedComponent);

    selectedComponentChanged(mySelectedComponent);
}

void SofaScene::setSelectedManipulator(sofa::qtquick::Manipulator* newSelectedManipulator)
{
    if(newSelectedManipulator == mySelectedManipulator)
        return;

    mySelectedManipulator = newSelectedManipulator;

    selectedManipulatorChanged(newSelectedManipulator);
}

static void appendManipulator(QQmlListProperty<sofa::qtquick::Manipulator> *property, sofa::qtquick::Manipulator *value)
{
    static_cast<QList<sofa::qtquick::Manipulator*>*>(property->data)->append(value);
}

static sofa::qtquick::Manipulator* atManipulator(QQmlListProperty<sofa::qtquick::Manipulator> *property, int index)
{
    return static_cast<QList<sofa::qtquick::Manipulator*>*>(property->data)->at(index);
}

static void clearManipulator(QQmlListProperty<sofa::qtquick::Manipulator> *property)
{
    static_cast<QList<sofa::qtquick::Manipulator*>*>(property->data)->clear();
}

static int countManipulator(QQmlListProperty<sofa::qtquick::Manipulator> *property)
{
    return static_cast<QList<sofa::qtquick::Manipulator*>*>(property->data)->size();
}

QQmlListProperty<sofa::qtquick::Manipulator> SofaScene::manipulators()
{
    return QQmlListProperty<sofa::qtquick::Manipulator>(this, &myManipulators, appendManipulator, countManipulator, atManipulator, clearManipulator);
}

double SofaScene::radius() const
{
    QVector3D min, max;
    computeBoundingBox(min, max);
    QVector3D diag = (max - min);

    return diag.length();
}

void SofaScene::computeBoundingBox(QVector3D& min, QVector3D& max) const
{
    SReal pmin[3], pmax[3];
    mySofaSimulation->computeTotalBBox(mySofaRootNode.get(), pmin, pmax);

    min = QVector3D(pmin[0], pmin[1], pmin[2]);
    max = QVector3D(pmax[0], pmax[1], pmax[2]);
}

void SofaScene::computeBoundingBox(QVector3D& min, QVector3D& max, const QList<SofaComponent*>& roots) const
{
    if(roots.isEmpty())
        return computeBoundingBox(min, max);

    SReal pmin[3], pmax[3];

    pmin[0] = pmin[1] = pmin[2] = 1e10;
    pmax[0] = pmax[1] = pmax[2] = -1e10;

    for(SofaComponent* sofaComponent : roots)
    {
        Node* node = dynamic_cast<Node*>(sofaComponent->base());
        if(!node)
            continue;

        sofa::core::ExecParams* params = sofa::core::ExecParams::defaultInstance();
        node->execute<UpdateBoundingBoxVisitor>( params );
        defaulttype::BoundingBox bb = node->f_bbox.getValue();
        for(int i=0; i<3; i++){
            pmin[i]= bb.minBBox()[i];
            pmax[i]= bb.maxBBox()[i];
        }
    }

    min = QVector3D(pmin[0], pmin[1], pmin[2]);
    max = QVector3D(pmax[0], pmax[1], pmax[2]);
}

QString SofaScene::dumpGraph() const
{
    QString dump;

    if(mySofaRootNode)
    {
        std::streambuf* backup(std::cout.rdbuf());

        std::ostringstream stream;
        std::cout.rdbuf(stream.rdbuf());
        mySofaSimulation->print(mySofaRootNode.get());
        std::cout.rdbuf(backup);

        dump += QString::fromStdString(stream.str());
    }

    return dump;
}

bool SofaScene::reinitComponent(const QString& path)
{
    QStringList pathComponents = path.split("/");

    Node::SPtr node = mySofaRootNode;
    unsigned int i = 0;
    while(i+1 < (unsigned int)pathComponents.size()) {
        if (pathComponents[i]=="@") {
            ++i;
            continue;
        }

        node = node->getChild(pathComponents[i].toStdString());
        if (!node) {
            msg_warning("SofaQtQuickGUI") << "Object path unknown:" << path.toStdString();
            return false;
        }
        ++i;
    }
    BaseObject* object = node->get<BaseObject>(pathComponents[i].toStdString());
    if(!object) {
        msg_warning("SofaQtQuickGUI") << "Object path unknown:" << path.toStdString();
        return false;
    }

    object->reinit();

    return true;
}

bool SofaScene::removeComponent(SofaComponent* sofaComponent)
{
    if(!sofaComponent)
        return false;

    // if component is an object
    BaseObject* baseObject = sofaComponent->base()->toBaseObject();
    if(baseObject)
    {
        BaseContext* baseContext = baseObject->getContext();
        baseContext->removeObject(baseObject);

        return true;
    }

    // if component is a node
    BaseNode* baseNode = sofaComponent->base()->toBaseNode();
    if(baseNode)
    {
        Node::SPtr node = static_cast<Node*>(baseNode);
        node->detachFromGraph();
        node->execute<simulation::DeleteVisitor>(sofa::core::ExecParams::defaultInstance());

        return true;
    }

    return false;
}


bool SofaScene::addNodeTo(SofaComponent* sofaComponent)
{
    msg_info("SofaScene") << "add a new Node !!! " ;

    if(!sofaComponent)
        return false;

    // if component is an object
    BaseObject* baseObject = sofaComponent->base()->toBaseObject();
    if(baseObject)
    {
        dmsg_info("SofaScene") << "This shouldn't happen" ;

        return false;
    }

    // if component is a node
    BaseNode* baseNode = sofaComponent->base()->toBaseNode();
    if(baseNode)
    {
        Node::SPtr node = static_cast<Node*>(baseNode);
        node->createChild("NEWNODE") ;
        return true;
    }

    return false;
}


bool SofaScene::areSameComponent(SofaComponent* sofaComponentA, SofaComponent* sofaComponentB)
{
    if(!sofaComponentA)
        return false;

    return sofaComponentA->isSame(sofaComponentB);
}

bool SofaScene::areInSameBranch(SofaComponent* sofaComponentA, SofaComponent* sofaComponentB)
{
    if(!sofaComponentA || !sofaComponentB)
        return false;

    BaseObject* baseObjectA = sofaComponentA->base()->toBaseObject();
    BaseObject* baseObjectB = sofaComponentB->base()->toBaseObject();

    if(!baseObjectA || !baseObjectB)
        return false;

    BaseNode* baseNodeA = baseObjectA->getContext()->toBaseNode();
    BaseNode* baseNodeB = baseObjectB->getContext()->toBaseNode();

    if(!baseNodeA || !baseNodeB)
        return false;

    if(baseNodeA == baseNodeB)
        return true;

    if(baseNodeA->hasAncestor(baseNodeB) || baseNodeB->hasAncestor(baseNodeA))
        return true;

    return false;
}

void SofaScene::sendGUIEvent(const QString& controlID, const QString& valueName, const QString& value)
{
    if(!mySofaRootNode)
        return;

    sofa::core::objectmodel::GUIEvent event(controlID.toUtf8().constData(), valueName.toUtf8().constData(), value.toUtf8().constData());
    mySofaRootNode->propagateEvent(sofa::core::ExecParams::defaultInstance(), &event);
}

QVariant SofaScene::linkValue(const sofa::core::objectmodel::BaseLink* link)
{
    QVariant value;

    if(!link)
        return value;

    value = QVariant::fromValue(QString::fromStdString(link->getValueString()));

    return value;
}

QVariantMap SofaScene::dataObject(const sofa::core::objectmodel::BaseData* data)
{
    QVariantMap object;

    if(!data)
    {
        object.insert("sofaData", QVariant::fromValue((SofaData*) nullptr));
        object.insert("name", "Invalid");
        object.insert("description", "");
        object.insert("type", "");
        object.insert("group", "");
        object.insert("properties", "");
        object.insert("link", "");
        object.insert("value", "");

        return object;
    }

    // TODO:
    QString type;
    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();

    QVariantMap properties;

    if(typeinfo->Text())
    {
        type = "string";
    }
    else if(typeinfo->Scalar())
    {
        type = "number";
        properties.insert("step", 0.1);
        properties.insert("decimals", 14);
    }
    else if(typeinfo->Integer())
    {
        if(std::string::npos != typeinfo->name().find("bool"))
        {
            type = "boolean";
            properties.insert("autoUpdate", true);
        }
        else
        {
            type = "number";
            properties.insert("decimals", 0);
            if(std::string::npos != typeinfo->name().find("unsigned"))
                properties.insert("min", 0);
        }
    }
    else
    {
        type = QString::fromStdString(data->getValueTypeString());
    }

    if(typeinfo->Container())
    {
        type = "array";
        int nbCols = typeinfo->size();

        properties.insert("cols", nbCols);
        if(typeinfo->FixedSize())
            properties.insert("static", true);

        const AbstractTypeInfo* baseTypeinfo = typeinfo->BaseType();
        if(baseTypeinfo->FixedSize())
            properties.insert("innerStatic", true);
    }

    /// DataFilename are use to stores path to files.
    const DataFileName* aDataFilename = dynamic_cast<const DataFileName*>(data) ;

    /// OptionsGroup are used to encode a finite set of alternatives.
    const Data<OptionsGroup>* anOptionGroup =  dynamic_cast<const Data<OptionsGroup>*>(data) ;

    /// OptionsGroup are used to encode a finite set of alternatives.
    const Data<RGBAColor>* aRGBAColor =  dynamic_cast<const Data<RGBAColor>*>(data) ;

    if(aDataFilename)
    {
        type = "FileName" ;
        properties.insert("url", QString::fromStdString(aDataFilename->getFullPath())) ;

        const std::string& directory = FileSystem::getParentDirectory( aDataFilename->getFullPath() ) ;
        dmsg_info("SofaScene") << directory ;
        properties.insert("folderurl",  QString::fromStdString(directory)) ;
    }
    else if(anOptionGroup)
    {
        type = "OptionsGroup";
        QStringList choices;

        const OptionsGroup& group = anOptionGroup->getValue();
        for(unsigned int i=0;i<group.size();++i)
        {
            choices.append(QString::fromStdString(group[i]));
        }
        properties.insert("choices", choices);
    }else if(aRGBAColor)
    {
                type = "RGBAColor";
    }

    QString widget(data->getWidget());
    if(!widget.isEmpty())
        type = widget;

    properties.insert("readOnly", data->isReadOnly());

    SofaData* sofaData = new SofaData(new SofaComponent(this, data->getOwner()), data);
    object.insert("sofaData", QVariant::fromValue(sofaData));
    object.insert("name", data->getName().c_str());
    object.insert("description", data->getHelp());
    object.insert("type", type);
    object.insert("group", data->getGroup());
    object.insert("properties", properties);
    object.insert("link", QString::fromStdString(data->getLinkPath()));
    object.insert("value", dataValue(data));

    return object;
}

QVariant SofaScene::dataValue(const BaseData* data)
{
    QVariant value;

    if(!data)
        return value;

    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();
    const void* valueVoidPtr = data->getValueVoidPtr();

    if(!typeinfo->Container())
    {
        if(typeinfo->Text())
            value = QString::fromStdString(typeinfo->getTextValue(valueVoidPtr, 0));
        else if(typeinfo->Scalar())
            value = typeinfo->getScalarValue(valueVoidPtr, 0);
        else if(typeinfo->Integer())
        {
            if(std::string::npos != typeinfo->name().find("bool"))
                value = 0 != typeinfo->getIntegerValue(valueVoidPtr, 0) ? true : false;
            else
                value = typeinfo->getIntegerValue(valueVoidPtr, 0);
        }
        else
        {
            value = QString::fromStdString(data->getValueString());
        }
    }
    else
    {
        int nbCols = typeinfo->size();
        int nbRows = typeinfo->size(data->getValueVoidPtr()) / nbCols;

        if(typeinfo->Text())
        {
            QVariantList values;
            values.reserve(nbRows);

            QVariantList subValues;
            subValues.reserve(nbCols);

            for(int j = 0; j < nbRows; j++)
            {
                subValues.clear();
                for(int i = 0; i < nbCols; i++)
                    subValues.append(QVariant::fromValue(QString::fromStdString((typeinfo->getTextValue(valueVoidPtr, j * nbCols + i)))));

                values.append(QVariant::fromValue(subValues));
            }

            value = values;
        }
        else if(typeinfo->Scalar())
        {
            QVariantList values;
            values.reserve(nbRows);

            QVariantList subValues;
            subValues.reserve(nbCols);

            for(int j = 0; j < nbRows; j++)
            {
                subValues.clear();
                for(int i = 0; i < nbCols; i++)
                    subValues.append(QVariant::fromValue(typeinfo->getScalarValue(valueVoidPtr, j * nbCols + i)));

                values.append(QVariant::fromValue(subValues));
            }

            value = values;
        }
        else if(typeinfo->Integer())
        {
            QVariantList values;
            values.reserve(nbRows);

            QVariantList subValues;
            subValues.reserve(nbCols);

            bool isBool = false;
            if(std::string::npos != typeinfo->name().find("bool"))
                isBool = true;

            for(int j = 0; j < nbRows; j++)
            {
                subValues.clear();

                if(isBool)
                    for(int i = 0; i < nbCols; i++)
                        subValues.append(QVariant::fromValue(0 != typeinfo->getIntegerValue(valueVoidPtr, j * nbCols + i) ? true : false));
                else
                    for(int i = 0; i < nbCols; i++)
                        subValues.append(QVariant::fromValue(typeinfo->getIntegerValue(valueVoidPtr, j * nbCols + i)));

                values.push_back(QVariant::fromValue(subValues));
            }

            value = values;
        }
        else
        {
            value = QString::fromStdString(data->getValueString());
        }
    }

    return value;
}

// TODO: WARNING : do not use data->read anymore but directly the correct set*Type*Value(...)
bool SofaScene::setDataValue(BaseData* data, const QVariant& value)
{
    if(!data)
        return false;

    const AbstractTypeInfo* typeinfo = data->getValueTypeInfo();

    if(!value.isNull())
    {
        QVariant finalValue = value;
        if(finalValue.userType() == qMetaTypeId<QJSValue>())
            finalValue = finalValue.value<QJSValue>().toVariant();

        if(QVariant::List == finalValue.type())
        {
            QSequentialIterable valueIterable = finalValue.value<QSequentialIterable>();

            int nbCols = typeinfo->size();
            int nbRows = typeinfo->size(data->getValueVoidPtr()) / nbCols;

            if(!typeinfo->Container())
            {
                msg_warning("SofaQtQuickGUI") << "Trying to set a list of values on a non-container data";
                return false;
            }

            if(valueIterable.size() != nbRows)
            {
                if(typeinfo->FixedSize())
                {
                    msg_warning("SofaQtQuickGUI") << "The new data should have the same size, should be" << nbRows << ", got" << valueIterable.size();
                    return false;
                }

                //typeinfo->setSize(data, valueIterable.size()); // CRASH and does not seems to be necessary since we call data->read, but maybe it will be necessary someday
            }

            if(typeinfo->Scalar())
            {
                QString dataString;
                for(int i = 0; i < valueIterable.size(); ++i)
                {
                    QVariant subFinalValue = valueIterable.at(i);
                    if(QVariant::List == subFinalValue.type())
                    {
                        QSequentialIterable subValueIterable = subFinalValue.value<QSequentialIterable>();
                        if(subValueIterable.size() != nbCols)
                        {
                            msg_warning("SofaQtQuickGUI") << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
                            return false;
                        }

                        for(int j = 0; j < subValueIterable.size(); ++j)
                        {
                            dataString += QString::number(subValueIterable.at(j).toDouble());
                            if(subValueIterable.size() - 1 != j)
                                dataString += ' ';
                        }
                    }
                    else
                    {
                        dataString += QString::number(subFinalValue.toDouble());
                    }

                    if(valueIterable.size() - 1 != i)
                        dataString += ' ';
                }

                data->read(dataString.toStdString());
            }
            else if(typeinfo->Integer())
            {
                QString dataString;
                for(int i = 0; i < valueIterable.size(); ++i)
                {
                    QVariant subFinalValue = valueIterable.at(i);
                    if(QVariant::List == subFinalValue.type())
                    {
                        QSequentialIterable subValueIterable = subFinalValue.value<QSequentialIterable>();
                        if(subValueIterable.size() != nbCols)
                        {
                            msg_warning("SofaQtQuickGUI") << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
                            return false;
                        }

                        for(int j = 0; j < subValueIterable.size(); ++j)
                        {
                            dataString += QString::number(subValueIterable.at(j).toLongLong());
                            if(subValueIterable.size() - 1 != j)
                                dataString += ' ';
                        }
                    }
                    else
                    {
                        dataString += QString::number(subFinalValue.toLongLong());
                    }

                    if(valueIterable.size() - 1 != i)
                        dataString += ' ';
                }

                data->read(dataString.toStdString());
            }
            else if(typeinfo->Text())
            {
                QString dataString;
                for(int i = 0; i < valueIterable.size(); ++i)
                {
                    QVariant subFinalValue = valueIterable.at(i);
                    if(QVariant::List == subFinalValue.type())
                    {
                        QSequentialIterable subValueIterable = subFinalValue.value<QSequentialIterable>();
                        if(subValueIterable.size() != nbCols)
                        {
                            msg_warning("SofaQtQuickGUI") << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
                            return false;
                        }

                        for(int j = 0; j < subValueIterable.size(); ++j)
                        {
                            dataString += subValueIterable.at(j).toString();
                            if(subValueIterable.size() - 1 != j)
                                dataString += ' ';
                        }
                    }
                    else
                    {
                        dataString += subFinalValue.toString();
                    }

                    if(valueIterable.size() - 1 != i)
                        dataString += ' ';
                }

                data->read(dataString.toStdString());
            }
            else
                data->read(value.toString().toStdString());
        }
        else if(QVariant::Map == finalValue.type())
        {
            msg_warning("SofaQtQuickGUI") << "Map type is not supported";
            return false;
        }
        else
        {
            if(typeinfo->Text())
                data->read(value.toString().toStdString());
            else if(typeinfo->Scalar())
                data->read(QString::number(value.toDouble()).toStdString());
            else if(typeinfo->Integer())
                data->read(QString::number(value.toLongLong()).toStdString());
            else
                data->read(value.toString().toStdString());
        }
    }
    else
        return false;

    return true;
}

bool SofaScene::setDataLink(BaseData* data, const QString& link)
{
    if(!data)
        return false;

    if(link.isEmpty())
        data->setParent(0);
    else
        data->setParent(link.toStdString());

    return data->getParent();
}

QVariant SofaScene::dataValue(const QString& path) const
{
    return onDataValueByPath(path);
}

QVariant SofaScene::dataValue(const SofaComponent* sofaComponent, const QString& name) const
{
    if(!sofaComponent)
        return QVariant();

    const Base* base = sofaComponent->base();
    if(!base)
        return QVariant();

    BaseData* data = base->findData(name.toStdString());
    return dataValue(data);
}

void SofaScene::setDataValue(const QString& path, const QVariant& value)
{
    onSetDataValueByPath(path, value);
}

void SofaScene::setDataValue(SofaComponent* sofaComponent, const QString& name, const QVariant& value)
{
    if(!sofaComponent)
        return;

    Base* base = sofaComponent->base();
    if(!base)
        return;

    BaseData* data = base->findData(name.toStdString());
    setDataValue(data, value);
}

static BaseData* FindData_Helper(BaseNode* node, const QString& path)
{
    BaseData* data = 0;

    if(node)
    {
        std::streambuf* backup(std::cerr.rdbuf());

        std::ostringstream stream;
        std::cerr.rdbuf(stream.rdbuf());
        node->findDataLinkDest(data, path.toStdString(), 0);
        std::cerr.rdbuf(backup);
    }

    return data;
}

SofaData* SofaScene::data(const QString& path)
{
    BaseData* data = FindData_Helper(mySofaRootNode.get(), path);
    if(!data)
        return nullptr;

    Base* base = data->getOwner();
    if(!base)
        return nullptr;

    return new SofaData(this, base, data);
}

SofaComponent* SofaScene::component(const QString& path)
{
    BaseData* data = FindData_Helper(mySofaRootNode.get(), path + ".name"); // search for the "name" data of the component (this data is always present if the component exist)

    if(!data)
        return nullptr;

    Base* base = data->getOwner();
    if(!base)
        return nullptr;

    return new SofaComponent(this, base);
}

void hasParentWithTypeName(const core::objectmodel::BaseClass* obj,
                           const std::string& c, bool& hasParent)
{
  for (auto parent : obj->parents)
  {
    if (c == parent->className) hasParent = true;
    hasParentWithTypeName(parent, c, hasParent);
  }
}

class GetObjectsByTypeVisitor : public Visitor
{
 public:
  GetObjectsByTypeVisitor(const QString& typeName)
      : Visitor(sofa::core::ExecParams::defaultInstance()), myTypeName(typeName)
  {
  }

  virtual Result processNodeTopDown(Node* node)
  {
    Node::ObjectIterator objectIt;
    for (objectIt = node->object.begin(); objectIt != node->object.end();
         ++objectIt)
      if (0 ==
          QString::fromStdString(objectIt->get()->getClassName())
              .compare(myTypeName, Qt::CaseInsensitive))
        myBaseObjects.append(objectIt->get());

    std::vector<core::objectmodel::BaseObject*> list;
    node->getContext()->get<core::objectmodel::BaseObject>(&list);
    for (core::objectmodel::BaseObject* obj : list)
    {
      bool hasParent = false;
      hasParentWithTypeName(obj->getClass(), myTypeName.toStdString(), hasParent);
      if (hasParent) myBaseObjects.append(obj);
    }

    return RESULT_CONTINUE;
  }

  QList<BaseObject*> baseObjects() const { return myBaseObjects; }

 private:
  QString myTypeName;
  QList<BaseObject*> myBaseObjects;
};

SofaComponent* SofaScene::componentByType(const QString& typeName)
{
    if(!mySofaSimulation || typeName.isEmpty())
        return nullptr;

    GetObjectsByTypeVisitor getObjectByTypeVisitor(typeName);
    mySofaRootNode->execute(getObjectByTypeVisitor);

    const QList<BaseObject*>& baseObjects = getObjectByTypeVisitor.baseObjects();
    if(baseObjects.isEmpty())
        return nullptr;

    BaseObject* firstBaseObject = baseObjects.first();

    return new SofaComponent(this, firstBaseObject);
}

sofa::qtquick::SofaComponentList* SofaScene::componentsByType(const QString& typeName)
{
    SofaComponentList* sofaComponents = new SofaComponentList(this);
    if(!mySofaSimulation || typeName.isEmpty())
        return sofaComponents;

    GetObjectsByTypeVisitor getObjectByTypeVisitor(typeName);
    mySofaRootNode->execute(getObjectByTypeVisitor);

    const QList<BaseObject*>& baseObjects = getObjectByTypeVisitor.baseObjects();
    for(BaseObject* baseObject : baseObjects)
        sofaComponents->append(new SofaComponent(this, baseObject));

    return sofaComponents;
}

bool SofaScene::componentExists(const sofa::core::objectmodel::Base* base) const
{
    return myBases.contains(base);
}

SofaComponent* SofaScene::root()
{
    if(!mySofaSimulation)
        return nullptr;

    Base* base = mySofaRootNode.get();
    if(!base)
        return nullptr;

    return new SofaComponent(this, base);
}

SofaComponent* SofaScene::visualStyleComponent()
{
    if(mySofaRootNode)
    {
        sofa::component::visualmodel::VisualStyle* visualStyle = nullptr;

        mySofaRootNode->get(visualStyle);
        if(visualStyle)
            return new SofaComponent(this, visualStyle);
    }

    return nullptr;
}

SofaComponent* SofaScene::retrievePythonScriptController(SofaComponent* context, const QString& derivedFrom, const QString& module)
{
    Base* base = context->base();
    if(!base)
        return nullptr;

    BaseContext* baseContext = dynamic_cast<BaseContext*>(base);
    if(!baseContext)
    {
        BaseObject* baseObject = dynamic_cast<BaseObject*>(base);
        baseContext = baseObject->getContext();

        if(!baseContext)
            return nullptr;
    }

    QList<PythonScriptController*> pythonScriptControllers;
    baseContext->get<PythonScriptController>(&pythonScriptControllers, BaseContext::Local);

    if(module.isEmpty())
    {
        for(PythonScriptController* pythonScriptController : pythonScriptControllers)
            if(derivedFrom.isEmpty() || pythonScriptController->isDerivedFrom(derivedFrom.toStdString()))
                return new SofaComponent(this, pythonScriptController);
    }
    else
    {
        for(PythonScriptController* pythonScriptController : pythonScriptControllers)
            if(derivedFrom.isEmpty() || pythonScriptController->isDerivedFrom(derivedFrom.toStdString(), module.toStdString()))
                return new SofaComponent(this, pythonScriptController);
    }


    return nullptr;
}

QVariant SofaScene::onDataValueByPath(const QString& path) const
{
    BaseData* data = FindData_Helper(mySofaRootNode.get(), path);

    if(!data)
    {
        msg_warning("SofaQtQuickGUI") << "DataPath unknown:" << path.toStdString();
        return QVariant();
    }

    return dataValue(data);
}

QVariant SofaScene::onDataValueByComponent(SofaComponent* sofaComponent, const QString& name) const
{
    return dataValue((const SofaComponent*) sofaComponent, name);
}

// arguments from JS have been packed in an array, we have to unpack them
static QVariant UnpackParameters_Helper(const QVariant& value)
{
    QVariant finalValue = value;
    if(finalValue.userType() == qMetaTypeId<QJSValue>())
        finalValue = finalValue.value<QJSValue>().toVariant();

    if(QVariant::List == finalValue.type())
    {
        QSequentialIterable valueIterable = finalValue.value<QSequentialIterable>();
        if(1 == valueIterable.size())
            finalValue = valueIterable.at(0);
    }

    return finalValue;
}

void SofaScene::onSetDataValueByPath(const QString& path, const QVariant& value)
{
    BaseData* data = FindData_Helper(mySofaRootNode.get(), path);

    if(!data)
    {
        msg_warning("SofaQtQuickGUI") << "DataPath unknown:" << path.toStdString();
    }
    else
    {
        if(!value.isNull())
            setDataValue(data, UnpackParameters_Helper(value));
    }
}

void SofaScene::onSetDataValueByComponent(SofaComponent* sofaComponent, const QString& name, const QVariant& value)
{
    return setDataValue(sofaComponent, name, UnpackParameters_Helper(value));
}

void SofaScene::reload()
{
    open();
}

void SofaScene::step()
{
    if(!isReady())
        return;

    emit stepBegin();

    bool oldAnimate = sofaRootNode()->getAnimate();
    if( !oldAnimate ) sofaRootNode()->setAnimate(true); // the 'animate' flag should be set even when performing only one step
    {
    sofa::helper::AdvancedTimer::TimerVar step("Animate");
    mySofaSimulation->animate(mySofaRootNode.get(), myDt);
    }
    if( !oldAnimate ) sofaRootNode()->setAnimate(false); // if was only a single step, so let's remove the 'animate' flag
    else setAnimate(sofaRootNode()->getAnimate()); // the simulation can be stopped while animating

    myVisualDirty = true;

    emit stepEnd();
}

void SofaScene::markVisualDirty()
{
    myVisualDirty = true;
}

void SofaScene::reset()
{
    if(!isReady())
        return;

    // TODO: ! NEED CURRENT OPENGL CONTEXT
    mySofaSimulation->reset(mySofaRootNode.get());
    myVisualDirty = true;
    emit reseted();
}

void SofaScene::draw(const SofaViewer& viewer, const QList<SofaComponent*>& roots) const
{
    if(!isReady())
        return;

    QList<sofa::simulation::Node*> nodes;
    nodes.reserve(roots.size());
    for(SofaComponent* sofaComponent : roots)
        if(sofaComponent)
        {
            sofa::core::objectmodel::Base* base = sofaComponent->base();
            if(base)
            {
                Node* node = down_cast<Node>(base->toBaseNode());
                if(!node->visualLoop)
                {
                    msg_warning("SofaQtQuickGUI")  << "SofaViewer: The node \"" << node->getPathName() << "\" has been selected for visualization but will be skipped because it contains no VisualManagerLoop";
                    continue;
                }

                nodes.append(node);
            }
        }

    if(nodes.isEmpty() && roots.isEmpty())
        nodes.append(mySofaRootNode.get());

    // prepare the sofa visual params
    sofa::core::visual::VisualParams* visualParams = sofa::core::visual::VisualParams::defaultInstance();
    if(visualParams)
    {
        GLint _viewport[4];
        GLdouble _mvmatrix[16], _projmatrix[16];

        glGetIntegerv(GL_VIEWPORT, _viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, _mvmatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, _projmatrix);

//        visualParams->viewport() = sofa::helper::fixed_array<int, 4>(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
//        visualParams->sceneBBox() = mySofaRootNode->f_bbox.getValue();
//        visualParams->setProjectionMatrix(_projmatrix);
//        visualParams->setModelViewMatrix(_mvmatrix);
    }

    if(myVisualDirty)
    {
        myVisualDirty = false;

        mySofaSimulation->updateVisual(mySofaRootNode.get());
    }

    for(sofa::simulation::Node* root : nodes)
    {
        if(!root)
            continue;

        mySofaSimulation->draw(sofa::core::visual::VisualParams::defaultInstance(), root);
    }

    // highlight selected components using a specific shader
    if(viewer.drawSelected())
    {
        Base* selectedBase = nullptr;
        if(mySelectedComponent)
            selectedBase = mySelectedComponent->base();

        if(selectedBase)
        {
            glDepthFunc(GL_LEQUAL);

            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-0.2f, 0.0f);

            myHighlightShaderProgram->bind();
            {
                VisualModel* visualModel = selectedBase->toVisualModel();
                if(visualModel)
                {
                    VisualStyle* visualStyle = nullptr;
                    visualModel->getContext()->get(visualStyle);

                    if(visualStyle)
                        visualStyle->fwdDraw(visualParams);

                    sofa::core::visual::tristate state = visualParams->displayFlags().getShowWireFrame();
                    visualParams->displayFlags().setShowWireFrame(true);

                    visualModel->drawVisual(visualParams);

                    visualParams->displayFlags().setShowWireFrame(state);

                    if(visualStyle)
                        visualStyle->bwdDraw(visualParams);
                }
                else
                {
                    TriangleModel* triangleModel = dynamic_cast<TriangleModel*>(selectedBase);
                    if(triangleModel)
                    {
                        VisualStyle* visualStyle = nullptr;
                        triangleModel->getContext()->get(visualStyle);

                        if(visualStyle)
                            visualStyle->fwdDraw(visualParams);

                        sofa::core::visual::tristate state = visualParams->displayFlags().getShowWireFrame();
                        visualParams->displayFlags().setShowWireFrame(true);

                        triangleModel->draw(visualParams);

                        visualParams->displayFlags().setShowWireFrame(state);

                        if(visualStyle)
                            visualStyle->bwdDraw(visualParams);
                    }
                }
            }
            myHighlightShaderProgram->release();

            glDisable(GL_POLYGON_OFFSET_LINE);

            glDepthFunc(GL_LESS);
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

    // draw manipulators
    if(viewer.drawManipulators())
        for(Manipulator* manipulator : myManipulators)
            if(manipulator)
                manipulator->draw(viewer);
}

SelectableSofaParticle* SofaScene::pickParticle(const QVector3D& origin, const QVector3D& direction, double distanceToRay, double distanceToRayGrowth, const QStringList& tags, const QList<SofaComponent*>& roots)
{
    SelectableSofaParticle* selectableSofaParticle = nullptr;

    std::list<Tag> tagList(tags.size());
    std::transform(tags.constBegin(), tags.constEnd(), tagList.begin(), [](const QString& tag) {return Tag(tag.toStdString());});

    sofa::simulation::MechanicalPickParticlesWithTagsVisitor pickVisitor(sofa::core::ExecParams::defaultInstance(),
                                                                        sofa::defaulttype::Vector3(origin.x(), origin.y(), origin.z()),
                                                                        sofa::defaulttype::Vector3(direction.x(), direction.y(), direction.z()),
                                                                        distanceToRay,
                                                                        distanceToRayGrowth,
                                                                        tagList,
                                                                        false);

    QList<sofa::simulation::Node*> nodes;
    nodes.reserve(roots.size());
    for(SofaComponent* sofaComponent : roots)
        if(sofaComponent)
        {
            sofa::core::objectmodel::Base* base = sofaComponent->base();
            if(base)
                nodes.append(down_cast<Node>(base->toBaseNode()));
        }

    if(nodes.isEmpty() && roots.isEmpty())
        nodes.append(sofaRootNode().get());

    for(sofa::simulation::Node* root : nodes)
    {
        if(!root)
            continue;

        pickVisitor.execute(root->getContext());

        if(!pickVisitor.particles.empty())
        {
            core::behavior::BaseMechanicalState* mstate;
            unsigned int indexCollisionElement;
            defaulttype::Vector3 point;
            SReal rayLength;
            pickVisitor.getClosestParticle( mstate, indexCollisionElement, point, rayLength );

            MechanicalObject3* mechanicalObject = dynamic_cast<MechanicalObject3*>(mstate);

            if(mechanicalObject)
            {
                selectableSofaParticle = new SelectableSofaParticle(SofaComponent(this, mechanicalObject), indexCollisionElement);
                break;
            }
        }
    }

    return selectableSofaParticle;
}

static QVector4D packPickingIndex(int i)
{
    return QVector4D((i & 0x000000FF) / 255.0, ((i & 0x0000FF00) >> 8) / 255.0, ((i & 0x00FF0000) >> 16) / 255.0, 0.0);
}

static int unpackPickingIndex(const std::array<unsigned char, 4>& i)
{
    return (i[0] | (i[1] << 8) | (i[2] << 16));
}

static bool HasTag_Helper(Base* base, const QStringList& tags)
{
    if(!base)
        return false;

    if(tags.isEmpty())
        return true;

    for(const QString& tag : tags)
        if(base->hasTag(Tag(tag.toStdString())))
            return true;

    return false;
}

Selectable* SofaScene::pickObject(const SofaViewer& viewer, const QPointF& ssPoint, const QStringList& tags, const QList<SofaComponent*>& roots)
{
    Selectable* selectable = nullptr;

    QList<sofa::simulation::Node*> nodes;
    nodes.reserve(roots.size());
    for(SofaComponent* sofaComponent : roots)
        if(sofaComponent)
        {
            sofa::core::objectmodel::Base* base = sofaComponent->base();
            if(base)
                nodes.append(down_cast<Node>(base->toBaseNode()));
        }

    if(nodes.isEmpty() && roots.isEmpty())
        nodes.append(mySofaRootNode.get());

    QSize nativeSize = viewer.nativeSize();
    if(!myPickingFBO || nativeSize != myPickingFBO->size())
    {
        delete myPickingFBO;
        myPickingFBO = new QOpenGLFramebufferObject(nativeSize, QOpenGLFramebufferObject::CombinedDepthStencil);
    }

    myPickingFBO->bind();
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);

        sofa::core::visual::VisualParams* visualParams = sofa::core::visual::VisualParams::defaultInstance();
        QVector<VisualModel*> visualModels;
        QVector<TriangleModel*> triangleModels;

// write object index

        int index = 1;

        myPickingShaderProgram->bind();
        {
            int indexLocation = myPickingShaderProgram->uniformLocation("index");

            if(nodes.isEmpty())
                nodes.append(sofaRootNode().get());

            // visual models

            for(sofa::simulation::Node* root : nodes)
            {
                if(!root)
                    continue;

                sofa::helper::vector<VisualModel*> currentVisualModels;
                root->getTreeObjects<VisualModel>(&currentVisualModels);

                if(currentVisualModels.empty())
                    continue;

                for(VisualModel* visualModel : currentVisualModels)
                {
                    if(HasTag_Helper(visualModel, tags))
                    {
                        myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(index));
                        visualModel->drawVisual(visualParams);

                        visualModels.append(visualModel);
                        index++;
                    }
                }
            }

            // triangle models

            for(sofa::simulation::Node* root : nodes)
            {
                if(!root)
                    continue;

                sofa::helper::vector<TriangleModel*> currentTriangleModels;
                root->getTreeObjects<TriangleModel>(&currentTriangleModels);

                if(currentTriangleModels.empty())
                    continue;

                for(TriangleModel* triangleModel : currentTriangleModels)
                {
                    if(HasTag_Helper(triangleModel, tags))
                    {
                        VisualStyle* visualStyle = nullptr;
                        triangleModel->getContext()->get(visualStyle);

                        myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(index));

                        if(visualStyle)
                            visualStyle->fwdDraw(visualParams);

                        triangleModel->draw(visualParams);

                        if(visualStyle)
                            visualStyle->bwdDraw(visualParams);

                        triangleModels.append(triangleModel);
                        index++;
                    }
                }
            }

            if(viewer.drawManipulators() && (tags.isEmpty() || tags.contains("manipulator", Qt::CaseInsensitive)))
                for(Manipulator* manipulator : myManipulators)
                {
                    if(manipulator)
                    {
                        myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(index));
                        manipulator->pick(viewer);
                    }

                    index++;
                }
        }
        myPickingShaderProgram->release();

        // read object index
        QPointF nativePoint = viewer.mapToNative(ssPoint);
        std::array<unsigned char, 4> indexComponents;
        glReadPixels(nativePoint.x(), nativePoint.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, indexComponents.data());

        index = unpackPickingIndex(indexComponents) - 1;

        if(-1 != index)
        {
            if(index < visualModels.size())
            {
                selectable = new SelectableSofaComponent(SofaComponent(this, visualModels[index]));
            }
            else
            {
                index -= visualModels.size();

                if(index < triangleModels.size())
                {
                    selectable = new SelectableSofaComponent(SofaComponent(this, triangleModels[index]));
                }
                else
                {
                    index -= triangleModels.size();

                    if(viewer.drawManipulators())
                        if((int) index < myManipulators.size())
                            selectable = new SelectableManipulator(*myManipulators[index]);
                }
            }
        }

        if(selectable)
        {
            float z = 1.0;
            glReadPixels(nativePoint.x(), nativePoint.y(), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
            selectable->setPosition(viewer.mapToWorld(ssPoint, z));
        }
        else if(-1 != index)
        {
            msg_warning("SofaQtQuickGUI") << "SofaScene::pickObject(...) return an incorrect object index";
        }
    }
    myPickingFBO->release();

    return selectable;
}

void SofaScene::onKeyPressed(char key)
{
    if(!isReady())
        return;

    sofa::core::objectmodel::KeypressedEvent keyEvent(key);
    sofaRootNode()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}

void SofaScene::onKeyReleased(char key)
{
    if(!isReady())
        return;

    sofa::core::objectmodel::KeyreleasedEvent keyEvent(key);
    sofaRootNode()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}

void SofaScene::addChild(Node* parent, Node* child)
{
    if(!child)
        return;

    myBases.insert(child);

    MutationListener::addChild(parent, child);
}

void SofaScene::removeChild(Node* parent, Node* child)
{
    if(!child)
        return;

    MutationListener::removeChild(parent, child);

    myBases.remove(child);
}

void SofaScene::addObject(Node* parent, BaseObject* object)
{
    if(!object || !parent)
        return;

    myBases.insert(object);

    MutationListener::addObject(parent, object);
}

void SofaScene::removeObject(Node* parent, BaseObject* object)
{
    if(!object || !parent)
        return;

    MutationListener::removeObject(parent, object);

    myBases.remove(object);
}

}

}
