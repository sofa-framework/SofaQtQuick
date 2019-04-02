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

#include <SofaQtQuickGUI/SofaScene.h>
#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/SelectableManipulator.h>

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
#include <SofaPython/SceneLoaderPY.h>
//#include <SofaPython/PythonScriptController.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/simulation/MechanicalVisitor.h>
#include <sofa/simulation/DeleteVisitor.h>

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

#include <array>
#include <sstream>
#include <qqml.h>
#include <QVector3D>
#include <QStack>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QUrl>
#include <QThread>
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
using namespace sofa::simulation;

typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3Types> MechanicalObject3;

SofaScene::SofaScene(QObject *parent) : QObject(parent),
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
    myManipulators(),
    mySelectedManipulator(nullptr),
    mySelectedComponent(nullptr)
{
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
    plugins.append("SofaPython");
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
    connect(this, &SofaScene::sourceChanged, this, &SofaScene::open);
    connect(this, &SofaScene::animateChanged, myStepTimer, [&](bool newAnimate) {newAnimate ? myStepTimer->start() : myStepTimer->stop();});
    connect(this, &SofaScene::statusChanged, this, &SofaScene::handleStatusChange);
    connect(myStepTimer, &QTimer::timeout, this, &SofaScene::step);
}

SofaScene::~SofaScene()
{
    delete m_scenechecker ;

    setSource(QUrl());

    if(mySofaSimulation == sofa::simulation::getSimulation())
        sofa::simulation::setSimulation(0);

    sofa::simulation::graph::cleanup();
}

bool LoaderProcess(SofaScene* sofaScene)
{
    if(!sofaScene || !sofaScene->sofaSimulation() || sofaScene->path().isEmpty())
        return false;

    sofaScene->sofaRootNode() = sofaScene->sofaSimulation()->load(sofaScene->path().toLatin1().constData());
    if( sofaScene->sofaRootNode() )
    {
        sofaScene->sofaSimulation()->init(sofaScene->sofaRootNode().get());

        if(sofaScene->sofaRootNode()->getAnimate() || sofaScene->defaultAnimate())
            sofaScene->setAnimate(true);

        /// Validating the scene using the following scene checker. This is usefull to
        /// warn user about common mistake like having duplicated names, missing required plugins
        /// using alias and API changes.
        sofaScene->m_scenechecker->validate(sofaScene->sofaRootNode().get());
        sofaScene->setStatus(SofaScene::Status::Ready);

        if(!sofaScene->pathQML().isEmpty())
        {
            std::cout << "VALUE... " << sofaScene->pathQML().toStdString() << std::endl << std::endl;
            sofaScene->setSourceQML(QUrl::fromLocalFile(sofaScene->pathQML()));
            sofaScene->addCanvas(QUrl::fromLocalFile(sofaScene->pathQML()));
        }
        return true;
    }
    else
    {
        sofaScene->setStatus(SofaScene::Status::Error);
        return false;
    }
}


class LoaderThread : public QThread
{
public:
    LoaderThread(SofaScene* sofaScene) :
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
    SofaScene*                      mySofaScene;
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
    unloadAllCanvas();

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
    if(currentAsynchronous)
    {
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
        if(!LoaderProcess(this))
            setStatus(Status::Error);
        else
        {
            setDt(mySofaRootNode->getDt());
        }
    }
    emit rootNodeChanged();
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

void SofaScene::setSelectedComponent(sofa::qtquick::SofaBase* newSelectedComponent)
{
    if(newSelectedComponent == mySelectedComponent)
        return;

    delete mySelectedComponent;
    mySelectedComponent = nullptr;

    if(newSelectedComponent)
        mySelectedComponent = new SofaBase(newSelectedComponent->base());

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


SofaComponent* SofaScene::addNodeTo(SofaComponent* sofaComponent)
{
    if(!sofaComponent)
        return nullptr;

    // if component is an object
    BaseObject* baseObject = sofaComponent->base()->toBaseObject();
    if(baseObject)
    {
        dmsg_info("SofaScene") << "This shouldn't happen" ;
        return nullptr;
    }

    // if component is a node
    BaseNode* baseNode = sofaComponent->base()->toBaseNode();
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
        return new SofaComponent(this, newnode.get());
    }

    return nullptr;
}

bool SofaScene::createAndAddComponentTo(SofaComponent* sofaComponent, QString name)
{
    if(!sofaComponent)
        return false;

    /// if sofaComponent is an object then the new component is a brother of this one.
    BaseObject* baseObject = sofaComponent->base()->toBaseObject();
    if(baseObject)
    {
        msg_fatal("SofaScene") << "TO IMPLEMENT" ;
        return true;
    }

    /// if sofaComponent is a node then the new component is a child of this one.
    BaseNode* baseNode = sofaComponent->base()->toBaseNode();
    if(baseNode)
    {
        //TODO(dmarchal): to do
        msg_fatal("SofaScene") << "TO IMPLEMENT" ;

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
        properties.insert("autoUpdate", true);
    }
    else if(typeinfo->Scalar())
    {
        type = "number";
        properties.insert("autoUpdate", true);
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
            properties.insert("autoUpdate", true);
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
        properties.insert("autoUpdate", true);

    }else if(aRGBAColor)
    {
        type = "RGBAColor";
        properties.insert("autoUpdate", true);
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

/// Returns a link object from its path. The path
/// must be composed of a prefix.linkname
QObject* SofaScene::link(const QString& fullpath)
{
    std::cout << "GET FROM PATH: " << fullpath.toStdString() << std::endl ;

    // search for the "name" data of the component (this data is always present if the component exist)
    QStringList splittedpath = fullpath.split('.') ;

    if(splittedpath.size() != 2)
        return nullptr ;

    BaseData* data = FindData_Helper(mySofaRootNode.get(), splittedpath[0] + ".name");
    if(!data)
        return nullptr;

    Base* base = data->getOwner();
    if(!base)
        return nullptr;

    BaseLink* link = base->findLink(splittedpath[1].toStdString()) ;

    return new SofaLink(this, base, link);
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

SofaComponent* SofaScene::node(const QString& path)
{
    Node* node = mySofaRootNode->getNodeInGraph(path.toStdString());
    if (!node)
        return nullptr;
    return new SofaComponent(this, node);
}

SofaComponent* SofaScene::component(const QString& path)
{
    for (auto& child : mySofaRootNode->child)
        std::cout << child->getName() << std::endl;
    Base::SPtr base;
    mySofaRootNode->get<Base>(base, path.toStdString());
    if (!base.get())
    {
        std::cout << "No component found for " << path.toStdString() << std::endl;
        return nullptr;
    }

    return new SofaComponent(this, base.get());
}

SofaBase* SofaScene::get(const QString& path)
{
    /// search for the "name" data of the component (this data is always present if the component exist)
    BaseData* data = FindData_Helper(mySofaRootNode.get(), path + ".name");

    if(!data)
        return nullptr;

    Base* base = data->getOwner();
    if(!base)
        return nullptr;

    return new SofaBase(base);
}

bool isInstanceOf(const core::objectmodel::BaseClass* obj, const std::string& c)
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

/*
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
}*/

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
    if( !oldAnimate ) sofaRootNode()->setAnimate(false); // if was only a sin step, so let's remove the 'animate' flag
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

void SofaScene::addCanvas(const QUrl& canvas)
{
    m_canvas.push_back(canvas);
    emit notifyCanvasChanged();
}

void SofaScene::unloadAllCanvas()
{
    m_canvas.clear();
    emit notifyCanvasChanged();
}

QUrlList SofaScene::readCanvas()
{
    return m_canvas;
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

void SofaScene::prepareSceneForDrawing()
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

void SofaScene::onMouseMove(double x, double y)
{
    if(!isReady())
        return;

    MouseEvent event(MouseEvent::Move, x, y);
    sofaRootNode()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &event);
}

void SofaScene::onMousePressed(int button, double x, double y)
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

void SofaScene::onMouseReleased(int button, double x, double y)
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


}

}
