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

#include <SofaQtQuickGUI/SofaBaseScene.h>
#include <SofaQtQuickGUI/SofaViewer.h>

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
#include <sofa/core/objectmodel/MouseEvent.h>
using sofa::core::objectmodel::MouseEvent ;

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

#include <SofaPython3/PythonEnvironment.h>
#include <SofaPython3/SceneLoaderPY3.h>
using sofapython3::PythonEnvironment;
namespace py = pybind11;

#include <SofaPython3/PythonFactory.h>
using sofapython3::PythonFactory;

#include <QmlUI/Canvas.h>
using qmlui::Canvas;

#include <SofaBaseVisual/VisualStyle.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/simulation/MechanicalVisitor.h>
#include <sofa/simulation/DeleteVisitor.h>

#include <sofa/simulation/DefaultAnimationLoop.h>
#include <sofa/simulation/DefaultVisualManagerLoop.h>
#include <SofaBaseVisual/InteractiveCamera.h>
using sofa::simulation::DefaultAnimationLoop;
using sofa::simulation::DefaultVisualManagerLoop;
using sofa::component::visualmodel::InteractiveCamera;
using sofa::core::objectmodel::BaseObject;

////////////////// Include the checkers to validate the scene and warn users ///////////////////////
#include <SofaGraphComponent/SceneCheckerVisitor.h>
using sofa::simulation::scenechecking::SceneCheckerVisitor;
#include <SofaGraphComponent/SceneCheckAPIChange.h>
using sofa::simulation::scenechecking::SceneCheckAPIChange;
#include <SofaGraphComponent/SceneCheckMissingRequiredPlugin.h>
using sofa::simulation::scenechecking::SceneCheckMissingRequiredPlugin;
#include <SofaGraphComponent/SceneCheckDuplicatedName.h>
using sofa::simulation::scenechecking::SceneCheckDuplicatedName;
#include <SofaGraphComponent/SceneCheckUsingAlias.h>
using sofa::simulation::scenechecking::SceneCheckUsingAlias;

#include <SofaSimulationGraph/DAGNode.h>
using sofa::simulation::graph::DAGNode;

#include <SofaQtQuickGUI/DataHelper.h>

#include <SofaQtQuickGUI/Bindings/SofaCoreBindingFactory.h>
using sofaqtquick::bindings::SofaCoreBindingFactory;

#include <SofaQtQuickGUI/SofaBaseApplication.h>
using sofaqtquick::SofaBaseApplication;


#include <array>
#include <sstream>
#include <qqml.h>
#include <QVector3D>
#include <QStack>
#include <QFile>
#include <QFileDialog>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QUrl>
#include <QThread>
#include <QRunnable>
#include <QGuiApplication>
#include <QOffscreenSurface>
#include <QFileDialog>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace sofaqtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core;
using namespace sofa::core::objectmodel;
using namespace sofa::core::visual;
using namespace sofa::component::visualmodel;
using namespace sofa::simulation;

typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3Types> MechanicalObject3;
typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Rigid3Types> MechanicalObject7;

SofaBaseScene::SofaBaseScene(QObject *parent) : QObject(parent),
    myStatus(Status::Null),
    mySource(),
    mySourceQML(),
    myPath(),
    myPathQML(),
    myVisualDirty(false),
    myDt(0.04),
    myAnimate(false),
    myDefaultAnimate(false),
    myAsynchronous(true),
    myPyQtForceSynchronous(true),
    mySofaSimulation(nullptr),
    myStepTimer(new QTimer(this)),
    mySelectedComponent(nullptr)
{
    std::cout << "SCENE CREATED ..... " << std::endl;
    m_scenechecker=new SceneCheckerVisitor(ExecParams::defaultInstance());
    m_scenechecker->addCheck(SceneCheckAPIChange::newSPtr());
    m_scenechecker->addCheck(SceneCheckDuplicatedName::newSPtr());
    m_scenechecker->addCheck(SceneCheckMissingRequiredPlugin::newSPtr());
    m_scenechecker->addCheck(SceneCheckUsingAlias::newSPtr());

    myStepTimer->setInterval(0);

    sofa::simulation::graph::init();
    mySofaSimulation = sofa::simulation::graph::getSimulation();
    mySofaRootNode = mySofaSimulation->createNewNode("root");

    // plugins
    QVector<QString> plugins;
    plugins.append("SofaPython3");
    plugins.append("QmlUI");
    plugins.append("SofaAllCommonComponents");
    for(const QString& plugin : plugins)
    {
        std::string s = plugin.toStdString();
        sofa::helper::system::PluginManager::getInstance().loadPlugin(s);
    }

    sofa::core::ExecParams::defaultInstance()->setAspectID(0);
    sofa::core::ObjectFactory::ClassEntry::SPtr classVisualModel;
    sofa::core::ObjectFactory::AddAlias("VisualModel", "OglModel", true, &classVisualModel);


    sofa::helper::system::PluginManager::getInstance().init();

    // connections
    connect(this, &SofaBaseScene::sourceChanged, this, &SofaBaseScene::open);
    connect(this, &SofaBaseScene::animateChanged, myStepTimer, [&](bool newAnimate) {newAnimate ? myStepTimer->start() : myStepTimer->stop();});
    connect(this, &SofaBaseScene::statusChanged, this, &SofaBaseScene::handleStatusChange);
    connect(myStepTimer, &QTimer::timeout, this, &SofaBaseScene::step);
    connect(this, &SofaBaseScene::cppGraphChanged, this, &SofaBaseScene::loadCppGraph);
}

SofaBaseScene::~SofaBaseScene()
{
    delete m_scenechecker ;

    setSource(QUrl());

    if(mySofaSimulation == sofa::simulation::getSimulation())
        sofa::simulation::setSimulation(0);

    sofa::simulation::graph::cleanup();
}







/// Scene-related File Menu methods:
void SofaBaseScene::newScene()
{
    unloadAllCanvas();
    setPathQML("");
    setSource(QUrl());
    setSourceQML(QUrl());

    // return now if a scene is already loading
    if(Status::Loading == myStatus)
        return;

    // reset properties
    setAnimate(false);
    SofaBaseApplication::SetSelectedComponent(nullptr);

    if(mySofaRootNode)
    {
        setStatus(Status::Unloading);
        aboutToUnload();
        mySofaSimulation->unload(mySofaRootNode);
    }

    sofa::simulation::graph::init();
    mySofaSimulation = sofa::simulation::graph::getSimulation();
    mySofaRootNode = mySofaSimulation->createNewNode("root");
    myCppGraph = new SofaBase(mySofaRootNode);
    setDt(mySofaRootNode->getDt());
    setStatus(Status::Ready);
    emit rootNodeChanged();
}

void SofaBaseScene::openScene(QUrl projectDir)
{
    QFileDialog::Options options;
    options = QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly;
    QString title = "Choose scene file to open";
    QString filters = "SofaScene files (*.xml *.scn *.py *.pyscn)";
    QUrl sceneUrl = QFileDialog::getOpenFileUrl(nullptr, title, projectDir.toLocalFile(), filters, nullptr, options);
    if (!sceneUrl.isEmpty() && sceneUrl.isValid())
        this->setSource(sceneUrl);
}

void SofaBaseScene::reloadScene()
{
    reload();
}

void SofaBaseScene::saveScene(QString sceneFile)
{
    QString file = source().path();
    msg_warning("runSofa2") << "path: "  << source().path().toStdString();
    if (sceneFile != "") {
        msg_warning("runSofa2") << "sceneFile: "  << sceneFile.toStdString();
        file = sceneFile;
    }
    SofaNode* root = new sofaqtquick::SofaNode(DAGNode::SPtr(static_cast<DAGNode*>(mySofaRootNode->toBaseNode())));
    QFile::copy(file, file + ".backup");
    sofapython3::PythonEnvironment::executePython([file, root]()
    {
        std::string ppath = file.toStdString();
        py::module SofaQtQuick = py::module::import("SofaQtQuick");
        SofaQtQuick.reload();

        py::object rootNode = PythonFactory::toPython(root->self());

        py::str file(ppath);
        msg_warning("runSofa2") << "Saving to..: "  << ppath;
        bool ret =  py::cast<bool>(SofaQtQuick.attr("saveAsPythonScene")(file, rootNode));
        if (ret) {
            msg_warning("runSofa2") << "File saved to "  << ppath;
        } else {
            msg_error("runSofa2") << "Could not save to file "  << ppath;
        }
    });
    return;
}

void SofaBaseScene::saveSceneAs(QUrl projectDir)
{
    QFileDialog::Options options;
    options = QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly;
    QString title = "Save scene as";
    QString filters = "SofaScene files (*.xml *.scn *.py *.pyscn";
    QUrl sceneUrl = QFileDialog::getSaveFileUrl(nullptr, title, projectDir.toLocalFile(), filters, nullptr, options);
    saveScene(sceneUrl.path());
}

void SofaBaseScene::exportSceneAs(QUrl /*projectDir*/)
{
    msg_error("SofaBaseScene") <<  "Not implemented yet...";
}















bool LoaderProcess(SofaBaseScene* sofaScene)
{
    if(!sofaScene || !sofaScene->sofaSimulation() || sofaScene->path().isEmpty())
        return false;

    Node::SPtr n = sofaScene->sofaSimulation()->load(sofaScene->path().toLatin1().toStdString());
    sofaScene->setSofaRootNode(n);
    if ( sofaScene->sofaRootNode().get() )
    {
        sofaScene->sofaSimulation()->init(sofaScene->sofaRootNode().get());

        if(sofaScene->sofaRootNode()->getAnimate() || sofaScene->defaultAnimate())
            sofaScene->setAnimate(true);

        /// Validating the scene using the following scene checker. This is usefull to
        /// warn user about common mistake like having duplicated names, missing required plugins
        /// using alias and API changes.
        sofaScene->m_scenechecker->validate(sofaScene->sofaRootNode().get());
        sofaScene->setStatus(SofaBaseScene::Status::Ready);

        if(!sofaScene->pathQML().isEmpty())
        {
            sofaScene->setSourceQML(QUrl::fromLocalFile(sofaScene->pathQML()));
            sofaScene->addCanvas(QUrl::fromLocalFile(sofaScene->pathQML()));
        }
        return true;
    }
    else
    {
        sofaScene->setStatus(SofaBaseScene::Status::Error);
        sofaScene->setSofaRootNode(sofa::simulation::Node::create("root"));
        return false;
    }
}


class LoaderThread : public QThread
{
public:
    LoaderThread(SofaBaseScene* sofaScene) :
        mySofaScene(sofaScene),
        myIsLoaded(false)
    {

    }

    void run()
    {
        myIsLoaded = LoaderProcess(mySofaScene);
    }

    bool isLoaded() const			{return myIsLoaded;}

private:
    SofaBaseScene*                      mySofaScene;
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

void SofaBaseScene::loadCppGraph()
{
    unloadAllCanvas();
    setPathQML("");
    setSourceQML(QUrl());

    // return now if a scene is already loading
    if(Status::Loading == myStatus)
        return;

    // reset properties
    setAnimate(false);
    SofaBaseApplication::SetSelectedComponent(nullptr);
//    setSelectedManipulator(nullptr);

    if(mySofaRootNode)
    {
        setStatus(Status::Unloading);
        aboutToUnload();
        mySofaSimulation->unload(mySofaRootNode);
    }

    Node* n = dynamic_cast<Node*>(myCppGraph->rawBase());
    if (n != nullptr)
        mySofaRootNode = sofa::simulation::Node::SPtr(n);

    emit rootNodeChanged();
}

void SofaBaseScene::open()
{
    // clear the qml interface
    unloadAllCanvas();

    setPathQML("");
    setSourceQML(QUrl());

    // return now if a scene is already loading

    if(Status::Loading == myStatus)
        return;

    // reset properties
    setAnimate(false);

    SofaBaseApplication::SetSelectedComponent(nullptr);
//    setSelectedManipulator(nullptr);

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

    bool currentAsynchronous = false;

    // set the qml interface

    QString qmlFilepath = finalFilename;
    int extensionIndex = qmlFilepath.lastIndexOf('.');
    if(-1 != extensionIndex)
        qmlFilepath = qmlFilepath.left(extensionIndex);

    qmlFilepath += ".qml";

    std::string finalQmlFilepath = qmlFilepath.toLatin1().constData();
    if(!sofa::helper::system::DataRepository.findFile(finalQmlFilepath, "", nullptr))
    {
        finalQmlFilepath = (finalFilename + ".qml").toLatin1().constData();
        if(!sofa::helper::system::DataRepository.findFile(finalQmlFilepath, "", nullptr))
            finalQmlFilepath.clear();
        else
            msg_deprecated("SofaQtQuickGUI") << "The extension format of your scene qml interface is deprecated, use directly ***.qml instead of ***.py.qml";
    }
    setPathQML(QString::fromStdString(finalQmlFilepath));

    // load the requested scene synchronously / asynchronously
    if(currentAsynchronous)
    {
        std::cout << " ...ASYNC LOADING... " << std::endl;
        LoaderThread* loaderThread = new LoaderThread(this);

        connect(loaderThread, &QThread::finished, this, [this, loaderThread]() {
            if(!loaderThread->isLoaded())
                setStatus(Status::Error);
            else
            {
                setDt(mySofaRootNode->getDt());
            }

            loaderThread->deleteLater();
        });

        loaderThread->start();
    }
    else
    {
        std::cout << " ...SYNCHRONOUS LOADING... " << std::endl;

        if(!LoaderProcess(this))
            setStatus(Status::Error);
        else
        {
            setDt(mySofaRootNode->getDt());
        }
    }
    emit rootNodeChanged();
}

void SofaBaseScene::handleStatusChange(SofaBaseScene::Status newStatus)
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

void SofaBaseScene::setStatus(Status newStatus)
{
    if(newStatus == myStatus)
        return;

    myStatus = newStatus;

    statusChanged(newStatus);
}

void SofaBaseScene::setHeader(const QString& newHeader)
{
    if(newHeader == myHeader || Status::Loading == myStatus)
        return;

    myHeader = newHeader;

    headerChanged(newHeader);
}

void SofaBaseScene::setSource(const QUrl& newSource)
{
    mySource = QUrl(newSource);
    sourceChanged(mySource);
}

void SofaBaseScene::setCppSceneGraph(SofaBase* newCppGraph)
{
    if (newCppGraph->base() == myCppGraph->base())
        return;
    myCppGraph = newCppGraph;

    cppGraphChanged(newCppGraph);
}

void SofaBaseScene::setSourceQML(const QUrl& newSourceQML)
{
    if(newSourceQML == mySourceQML)
        return;

    mySourceQML = newSourceQML;

    sourceQMLChanged(newSourceQML);
}

void SofaBaseScene::setPath(const QString& newPath)
{
    if(newPath == myPath)
        return;

    myPath = newPath;

    pathChanged(newPath);
}

void SofaBaseScene::setPathQML(const QString& newPathQML)
{
    if(newPathQML == myPathQML)
        return;

    myPathQML = newPathQML;

    pathQMLChanged(newPathQML);
}

void SofaBaseScene::setDt(double newDt)
{
    if(newDt == myDt)
        return;

    myDt = newDt;

    dtChanged(newDt);
}

void SofaBaseScene::setAnimate(bool newAnimate)
{
    if(newAnimate == myAnimate)
        return;

    myAnimate = newAnimate;
    if(sofaSimulation() && sofaRootNode())
        sofaRootNode()->setAnimate(myAnimate);

    animateChanged(newAnimate);
}

void SofaBaseScene::setDefaultAnimate(bool newDefaultAnimate)
{
    if(newDefaultAnimate == myDefaultAnimate)
        return;

    myDefaultAnimate = newDefaultAnimate;

    defaultAnimateChanged(newDefaultAnimate);
}

void SofaBaseScene::setAsynchronous(bool newAsynchronous)
{
    if(newAsynchronous == myAsynchronous)
        return;

    myAsynchronous = newAsynchronous;

    asynchronousChanged(newAsynchronous);
}

void SofaBaseScene::setPyQtForceSynchronous(bool newPyQtForceSynchronous)
{
    if(newPyQtForceSynchronous == myPyQtForceSynchronous)
        return;

    myPyQtForceSynchronous = newPyQtForceSynchronous;

    pyQtForceSynchronousChanged(newPyQtForceSynchronous);
}


double SofaBaseScene::radius() const
{
    QVector3D min, max;
    computeBoundingBox(min, max);
    QVector3D diag = (max - min);

    return diag.length();
}

void SofaBaseScene::computeBoundingBox(QVector3D& min, QVector3D& max) const
{
    SReal pmin[3], pmax[3];
    mySofaSimulation->computeTotalBBox(mySofaRootNode.get(), pmin, pmax);

    min = QVector3D(pmin[0], pmin[1], pmin[2]);
    max = QVector3D(pmax[0], pmax[1], pmax[2]);
}

void SofaBaseScene::computeBoundingBox(QVector3D& min, QVector3D& max, const QList<SofaBase*>& roots) const
{
    if(roots.isEmpty())
        return computeBoundingBox(min, max);

    SReal pmin[3], pmax[3];

    pmin[0] = pmin[1] = pmin[2] = 1e10;
    pmax[0] = pmax[1] = pmax[2] = -1e10;

    for(SofaBase* SofaBase : roots)
    {
        Node* node = dynamic_cast<Node*>(SofaBase->rawBase());
        if(!node)
            continue;

        sofa::core::ExecParams* params = sofa::core::ExecParams::defaultInstance();
        node->execute<UpdateBoundingBoxVisitor>( params );
        sofa::defaulttype::BoundingBox bb = node->f_bbox.getValue();
        for(int i=0; i<3; i++){
            pmin[i]= bb.minBBox()[i];
            pmax[i]= bb.maxBBox()[i];
        }
    }

    min = QVector3D(pmin[0], pmin[1], pmin[2]);
    max = QVector3D(pmax[0], pmax[1], pmax[2]);
}

QString SofaBaseScene::dumpGraph() const
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

bool SofaBaseScene::reinitComponent(const QString& path)
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

bool SofaBaseScene::removeComponent(SofaBase* base)
{
    if(!base)
        return false;

    // if component is an object
    BaseObject* baseObject = base->rawBase()->toBaseObject();
    if(baseObject)
    {
        BaseContext* baseContext = baseObject->getContext();
        baseContext->removeObject(baseObject);

        return true;
    }

    // if component is a node
    BaseNode* baseNode = base->base()->toBaseNode();
    if(baseNode)
    {
        Node::SPtr node = static_cast<Node*>(baseNode);
        node->detachFromGraph();
        node->execute<sofa::simulation::DeleteVisitor>(sofa::core::ExecParams::defaultInstance());

        return true;
    }

    return false;
}

SofaBase* SofaBaseScene::addNodeTo(SofaBase* SofaBase)
{
    if(!SofaBase)
        return nullptr;

    // if component is an object
    BaseObject* baseObject = SofaBase->base()->toBaseObject();
    if(baseObject)
    {
        dmsg_info("SofaScene") << "This shouldn't happen" ;
        return nullptr;
    }

    // if component is a node
    BaseNode* baseNode = SofaBase->base()->toBaseNode();
    if(baseNode)
    {
        Node::SPtr node = static_cast<Node*>(baseNode);
        Node::SPtr newnode;
        std::string name = "NEWNODE" ;
        if(node->getChild(name)){
            int id=1;
            while(node->getChild( name+std::to_string(id) )){id++;}
            newnode = node->createChild(name+std::to_string(id)) ;
        }else{
            newnode = node->createChild(name) ;
        }
        return new sofaqtquick::bindings::SofaBase(newnode);
    }

    return nullptr;
}

void SofaBaseScene::addExistingNodeTo(sofaqtquick::bindings::SofaBase* SofaBase, sofaqtquick::bindings::SofaBase* sofaNode)
{
    if(!SofaBase)
        return;

    // if component is an object
    BaseObject* baseObject = SofaBase->base()->toBaseObject();
    if(baseObject)
    {
        dmsg_info("SofaScene") << "Can't add a node to a Graph Component! only to nodes" ;
        return;
    }

    // if component is a node
    BaseNode* baseNode = SofaBase->base()->toBaseNode();
    if(baseNode)
    {
        Node::SPtr node = static_cast<Node*>(baseNode);
        std::string name = sofaNode->base()->getName();
        if(node->getChild(name)) {
            int id=1;
            while(node->getChild( name+std::to_string(id) )){id++;}
            sofaNode->base()->setName(name + std::to_string(id));
            sofaNode->base()->getName();
            node->addChild(sofaNode->base()->toBaseNode());
        }
        else
            node->addChild(sofaNode->base()->toBaseNode());
        sofa::simulation::getSimulation()->init(static_cast<Node*>(node->getContext()->getRootContext()->toBaseNode()));
    }
}

bool SofaBaseScene::createAndAddComponentTo(SofaBase* SofaBase, QString name)
{
    if(!SofaBase)
        return false;

    /// if SofaBase is an object then the new component is a brother of this one.
    BaseObject* baseObject = SofaBase->base()->toBaseObject();
    if(baseObject)
    {
        msg_fatal("SofaScene") << "TO IMPLEMENT" ;
        return true;
    }

    /// if SofaBase is a node then the new component is a child of this one.
    BaseNode* baseNode = SofaBase->base()->toBaseNode();
    if(baseNode)
    {
        //TODO(dmarchal): to do
        msg_fatal("SofaScene") << "TO IMPLEMENT" ;

        return true;
    }

    return false;
}


bool SofaBaseScene::areSameComponent(SofaBase* SofaBaseA, SofaBase* SofaBaseB)
{
    if(!SofaBaseA)
        return false;

    return SofaBaseA->isSame(SofaBaseB);
}

bool SofaBaseScene::areInSameBranch(SofaBase* SofaBaseA, SofaBase* SofaBaseB)
{
    if(!SofaBaseA || !SofaBaseB)
        return false;

    BaseObject* baseObjectA = SofaBaseA->base()->toBaseObject();
    BaseObject* baseObjectB = SofaBaseB->base()->toBaseObject();

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

void SofaBaseScene::sendGUIEvent(const QString& controlID, const QString& valueName, const QString& value)
{
    if(!mySofaRootNode)
        return;

    sofa::core::objectmodel::GUIEvent event(controlID.toUtf8().constData(), valueName.toUtf8().constData(), value.toUtf8().constData());
    mySofaRootNode->propagateEvent(sofa::core::ExecParams::defaultInstance(), &event);
}

QVariant SofaBaseScene::linkValue(const sofa::core::objectmodel::BaseLink* link)
{
    QVariant value;

    if(!link)
        return value;

    value = QVariant::fromValue(QString::fromStdString(link->getValueString()));

    return value;
}

QVariantMap SofaBaseScene::dataObject(const sofa::core::objectmodel::BaseData* data)
{
    dmsg_deprecated("SofaScene::dataObject") << "is a deprecated method. Please use the one in helper";
}

QVariant SofaBaseScene::dataValue(const BaseData* data)
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
bool SofaBaseScene::setDataValue(BaseData* data, const QVariant& value)
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

bool SofaBaseScene::setDataLink(BaseData* data, const QString& link)
{
    if(!data)
        return false;

    if(link.isEmpty())
        data->setParent(nullptr);
    else
        data->setParent(link.toStdString());

    return data->getParent();
}

QVariant SofaBaseScene::dataValue(const QString& path) const
{
    return onDataValueByPath(path);
}

QVariant SofaBaseScene::dataValue(const SofaBase* SofaBase, const QString& name) const
{
    if(!SofaBase)
        return QVariant();

    const Base* base = SofaBase->rawBase();
    if(!base)
        return QVariant();

    BaseData* data = base->findData(name.toStdString());
    return dataValue(data);
}

void SofaBaseScene::setDataValue(const QString& path, const QVariant& value)
{
    onSetDataValueByPath(path, value);
}

void SofaBaseScene::setDataValue(SofaBase* SofaBase, const QString& name, const QVariant& value)
{
    if(!SofaBase)
        return;

    Base* base = SofaBase->rawBase();
    if(!base)
        return;

    BaseData* data = base->findData(name.toStdString());
    setDataValue(data, value);
}

/// Returns a link object from its path. The path
/// must be composed of a prefix.linkname
SofaLink* SofaBaseScene::link(const QString& fullpath)
{
    // search for the "name" data of the component (this data is always present if the component exist)
    QStringList splittedpath = fullpath.split('.') ;

    if(splittedpath.size() != 2)
        return nullptr ;

    BaseData* data = sofaqtquick::helper::findData(mySofaRootNode.get(), splittedpath[0] + ".name");
    if(!data)
        return nullptr;

    Base* base = data->getOwner();
    if(!base)
        return nullptr;

    BaseLink* link = base->findLink(splittedpath[1].toStdString()) ;

    return new SofaLink(link);
}

sofaqtquick::bindings::SofaData* SofaBaseScene::data(const QString& path)
{
    BaseData* data = sofaqtquick::helper::findData(mySofaRootNode.get(), path);
    if(!data)
        return nullptr;

    Base* base = data->getOwner();
    if(!base)
        return nullptr;

    return new sofaqtquick::bindings::SofaData(data);
}

SofaBase* SofaBaseScene::node(const QString& path)
{
    Node* node = mySofaRootNode->getNodeInGraph(path.toStdString());
    if (!node)
        return nullptr;
    return new SofaBase(node);
}

SofaBase* SofaBaseScene::component(const QString& path)
{
    QString p = path;

    return get(p.remove(0,1));
}

SofaBase* SofaBaseScene::get(const QString& path)
{
    /// search for the "name" data of the component (this data is always present if the component exist)
    BaseData* data = sofaqtquick::helper::findData(mySofaRootNode.get(), "@" + path + ".name");

    if(!data)
        return nullptr;

    Base* base = data->getOwner();
    if(!base)
        return nullptr;

    return SofaCoreBindingFactory::wrap(base);
}

bool isInstanceOf(const sofa::core::objectmodel::BaseClass* obj, const std::string& c)
{
    if (obj->className == c)
        return true;

    for (auto parent : obj->parents)
    {
        if( isInstanceOf(parent, c) )
            return true;
    }
    return false ;
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
        for (objectIt = node->object.begin(); objectIt != node->object.end(); ++objectIt){
            BaseObject::SPtr obj = *objectIt;
            if (isInstanceOf(obj->getClass(), myTypeName.toStdString()))
            {
                std::cout << obj->getClass()->className << std::endl;
                myBaseObjects.append(objectIt->get());
            }
        }

        return RESULT_CONTINUE;
    }

    QList<BaseObject*> baseObjects() const { return myBaseObjects; }

private:
    QString myTypeName;
    QList<BaseObject*> myBaseObjects;
};

SofaBaseObject* SofaBaseScene::componentByType(const QString& typeName)
{
    if(!mySofaSimulation || typeName.isEmpty())
    {
        msg_error("error") << "Can't retrieve component of type: " << typeName.toStdString() << ". no Sofa Simulation set?";
        return nullptr;
    }

    GetObjectsByTypeVisitor getObjectByTypeVisitor(typeName);
    mySofaRootNode->execute(getObjectByTypeVisitor);

    const QList<BaseObject*>& baseObjects = getObjectByTypeVisitor.baseObjects();
    if(baseObjects.isEmpty()) {
        msg_error("error") << "No Component of type " << typeName.toStdString() << " in this scene";
        return nullptr;
    }

    BaseObject* firstBaseObject = baseObjects.first();

    msg_error("error") << "Found " << firstBaseObject->getName() << " of type  " << typeName.toStdString() << " in this scene";
    return new SofaBaseObject(firstBaseObject);
}

SofaBaseObjectList* SofaBaseScene::componentsByType(const QString& typeName)
{
    sofaqtquick::bindings::SofaBaseObjectList* SofaBases = new sofaqtquick::bindings::SofaBaseObjectList();
    if(!mySofaSimulation || typeName.isEmpty())
        return SofaBases;

    GetObjectsByTypeVisitor getObjectByTypeVisitor(typeName);
    mySofaRootNode->execute(getObjectByTypeVisitor);

    const QList<BaseObject*>& baseObjects = getObjectByTypeVisitor.baseObjects();

    for(BaseObject* baseObject : baseObjects)
        SofaBases->append(new bindings::SofaBaseObject(baseObject));

    return SofaBases;
}

sofaqtquick::bindings::SofaNode* SofaBaseScene::root()
{
    if(!mySofaSimulation)
        return nullptr;

    Base* base = mySofaRootNode.get();
    if(!base)
        return nullptr;

    return new sofaqtquick::bindings::SofaNode(DAGNode::SPtr(dynamic_cast<DAGNode*>(base->toBaseNode())));
}

SofaBase* SofaBaseScene::visualStyleComponent()
{
    if(mySofaRootNode)
    {
        sofa::component::visualmodel::VisualStyle* visualStyle = nullptr;

        mySofaRootNode->get(visualStyle);
        if(visualStyle)
            return new SofaBase(visualStyle);
    }

    return nullptr;
}

QVariant SofaBaseScene::onDataValueByPath(const QString& path) const
{
    BaseData* data = sofaqtquick::helper::findData(mySofaRootNode.get(), path);

    if(!data)
    {
        msg_warning("SofaQtQuickGUI") << "DataPath unknown:" << path.toStdString();
        return QVariant();
    }

    return dataValue(data);
}

QVariant SofaBaseScene::onDataValueByComponent(SofaBase* SofaBase, const QString& name) const
{
    return dataValue((const sofaqtquick::bindings::SofaBase*) SofaBase, name);
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

void SofaBaseScene::onSetDataValueByPath(const QString& path, const QVariant& value)
{
    BaseData* data = sofaqtquick::helper::findData(mySofaRootNode.get(), path);

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

void SofaBaseScene::onSetDataValueByComponent(SofaBase* SofaBase, const QString& name, const QVariant& value)
{
    return setDataValue(SofaBase, name, UnpackParameters_Helper(value));
}

void SofaBaseScene::reload()
{
    open();
    emit rootNodeChanged();
}

void SofaBaseScene::step()
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
    if( !oldAnimate ) sofaRootNode()->setAnimate(false); // if was only a sin step, so let's remove the 'animate' flag
    else setAnimate(sofaRootNode()->getAnimate()); // the simulation can be stopped while animating

    myVisualDirty = true;

    emit stepEnd();
}

void SofaBaseScene::markVisualDirty()
{
    myVisualDirty = true;
}

void SofaBaseScene::reset()
{
    if(!isReady())
        return;

    // TODO: ! NEED CURRENT OPENGL CONTEXT
    mySofaSimulation->reset(mySofaRootNode.get());
    myVisualDirty = true;
    emit reseted();
}

void SofaBaseScene::addCanvas(const QUrl& canvas)
{
    Canvas::SPtr newCanvas = sofa::core::objectmodel::New<Canvas>();
    newCanvas->d_qmlFile.setValue(canvas.path().toStdString());
    sofaqtquick::bindings::SofaBaseObject* bo = new sofaqtquick::bindings::SofaBaseObject(newCanvas);
    m_canvas.push_back(bo);
    mySofaRootNode->addObject(newCanvas);
    emit notifyCanvasChanged();
}

void SofaBaseScene::unloadAllCanvas()
{
    m_canvas.clear();
    emit notifyCanvasChanged();
}

QList<QObject*> SofaBaseScene::readCanvas()
{
    return m_canvas;
}

class GetCanvasVisitor : public sofa::simulation::Visitor
{
public:
    GetCanvasVisitor(const sofa::core::ExecParams* params = sofa::core::ExecParams::defaultInstance())
        : sofa::simulation::Visitor(params),
          needsRefresh(false)
    {
    }

    sofa::simulation::Visitor::Result processNodeTopDown(Node* node) override
    {
        for (auto obj : node->object)
            if (obj->getClassName() == "Canvas")
            {
                qmlui::Canvas* canvas = dynamic_cast<qmlui::Canvas*>(obj.get());

                if (canvas->d_qmlFile.getFullPath() == "")
                {
                    if (canvas->d_lastModified.getValue() == 0)
                        continue;
                    else canvas->d_lastModified = 0;
                }

                QFileInfo f(QString::fromStdString(canvas->d_qmlFile.getFullPath()));

                if (f.lastModified().toTime_t() != canvas->d_lastModified.getValue())
                    canvas->d_lastModified.setValue(f.lastModified().toTime_t());
                if (canvas->tracker.hasChanged(canvas->d_lastModified) ||
                        canvas->tracker.hasChanged(canvas->d_qmlFile))
                    needsRefresh = true;
                canvas->tracker.clean();
                m_canvases.append(new sofaqtquick::bindings::SofaBaseObject(obj));
            }
        return RESULT_CONTINUE;
    }

    const char* getClassName() const override { return "GetCanvasVisitor"; }

    QList<QObject*>& getCanvases() { return m_canvases; }
    bool needsRefresh;

private:
    QList<QObject*> m_canvases;
};

void SofaBaseScene::checkForCanvases()
{
    if (!mySofaRootNode.get()) {
        m_canvas.clear();
    } else {
        GetCanvasVisitor visitor;
        mySofaRootNode->executeVisitor(&visitor);
        QList<QObject*> newCanvases = visitor.getCanvases();
        if (newCanvases.size() != m_canvas.size() || visitor.needsRefresh)
        {
            m_canvas = newCanvases;
        }
        else return;
    }
    emit notifyCanvasChanged();
}

SelectableSofaParticle* SofaBaseScene::pickParticle(const QVector3D& origin, const QVector3D& direction, double distanceToRay, double distanceToRayGrowth, const QStringList& tags, const QList<SofaBase*>& roots)
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
    for(SofaBase* SofaBase : roots)
        if(SofaBase)
        {
            sofa::core::objectmodel::Base* base = SofaBase->rawBase();
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
            sofa::core::behavior::BaseMechanicalState* mstate;
            unsigned int indexCollisionElement;
            sofa::defaulttype::Vector3 point;
            SReal rayLength;
            pickVisitor.getClosestParticle( mstate, indexCollisionElement, point, rayLength );

            MechanicalObject3* mechanicalObject = dynamic_cast<MechanicalObject3*>(mstate);
            if(mechanicalObject)
            {
                selectableSofaParticle = new SelectableSofaParticle(new sofaqtquick::bindings::SofaBaseObject(mechanicalObject), indexCollisionElement);
                break;
            }
            MechanicalObject7* rigidObject = dynamic_cast<MechanicalObject7*>(mstate);
            if(rigidObject)
            {
                selectableSofaParticle = new SelectableSofaParticle(new sofaqtquick::bindings::SofaBaseObject(rigidObject), indexCollisionElement);
                break;
            }
        }
    }

    return selectableSofaParticle;
}

void SofaBaseScene::prepareSceneForDrawing()
{
    if(myTextureAreDirty)
    {
        mySofaSimulation->initTextures(mySofaRootNode.get());
        myTextureAreDirty=false;
    }

    if(!myVisualDirty)
        return ;

   mySofaSimulation->updateVisual(mySofaRootNode.get());
    myVisualDirty = false;
}

void SofaBaseScene::onMouseMove(double x, double y)
{
    if(!isReady())
        return;

    MouseEvent event(MouseEvent::Move, x, y);
    sofaRootNode()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &event);
}

void SofaBaseScene::onMousePressed(int button, double x, double y)
{
    if(!isReady())
        return;

    MouseEvent::State s;
    switch(button)
    {
    case 1:
        s = MouseEvent::LeftPressed ;
        break;
    case 2:
        s = MouseEvent::RightPressed ;
        break;
    case 4:
        s = MouseEvent::MiddlePressed ;
        break;
    default:
        s = MouseEvent::AnyExtraButtonPressed ;
        break;
    }
    MouseEvent event(s, x, y);
    sofaRootNode()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &event);

}

void SofaBaseScene::onMouseReleased(int button, double x, double y)
{
    MouseEvent::State s;
    switch(button)
    {
    case 1:
        s = MouseEvent::LeftReleased ;
        break;
    case 2:
        s = MouseEvent::RightReleased ;
        break;
    case 4:
        s = MouseEvent::MiddleReleased ;
        break;
    default:
        s = MouseEvent::AnyExtraButtonReleased ;
        break;
    }
    MouseEvent event(s, x, y);
    sofaRootNode()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &event);
}

void SofaBaseScene::onKeyPressed(char key)
{
    if(!isReady())
        return;

    sofa::core::objectmodel::KeypressedEvent keyEvent(key);
    sofaRootNode()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}

void SofaBaseScene::onKeyReleased(char key)
{
    if(!isReady())
        return;

    sofa::core::objectmodel::KeyreleasedEvent keyEvent(key);
    sofaRootNode()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}


}
