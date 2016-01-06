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

#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/core/objectmodel/GUIEvent.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/PluginManager.h>
#include <sofa/helper/cast.h>
#include <sofa/simulation/graph/graph.h>
#include <sofa/simulation/graph/DAGSimulation.h>
#include <sofa/simulation/graph/init.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <sofa/core/visual/VisualModel.h>
#include <sofa/helper/system/glut.h>
#include <SofaPython/SceneLoaderPY.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaMeshCollision/TriangleModel.h>
#include <SofaComponentCommon/initComponentCommon.h>
#include <SofaComponentBase/initComponentBase.h>
#include <SofaComponentGeneral/initComponentGeneral.h>
#include <SofaComponentAdvanced/initComponentAdvanced.h>
#include <SofaComponentMisc/initComponentMisc.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/DeleteVisitor.h>

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
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QRunnable>
#include <QGuiApplication>
#include <QOffscreenSurface>

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::objectmodel;
using namespace sofa::core::visual;
using namespace sofa::component::visualmodel;
using namespace sofa::component::collision;
using namespace sofa::simulation;

typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3Types> MechanicalObject3;

SofaScene::SofaScene(QObject *parent) : QObject(parent), MutationListener(),
	myStatus(Status::Null),
	mySource(),
    mySourceQML(),
    myPathQML(),
    myVisualDirty(false),
	myDt(0.04),
	myPlay(false),
    myAsynchronous(true),
    myPyQtSynchronous(true),
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

	sofa::helper::system::PluginManager::getInstance().init();

	// connections
	connect(this, &SofaScene::sourceChanged, this, &SofaScene::open);
	connect(this, &SofaScene::playChanged, myStepTimer, [&](bool newPlay) {newPlay ? myStepTimer->start() : myStepTimer->stop();});
    connect(this, &SofaScene::statusChanged, this, &SofaScene::handleStatusChange);
    connect(this, &SofaScene::aboutToUnload, this, [&]() {myBases.clear();});

    connect(myStepTimer, &QTimer::timeout, this, &SofaScene::step);
}

SofaScene::~SofaScene()
{
    setSelectedComponent(nullptr);

	if(mySofaSimulation == sofa::simulation::getSimulation())
		sofa::simulation::setSimulation(0);

    sofa::simulation::graph::cleanup();
}

bool LoaderProcess(SofaScene* sofaScene, const QString& sofaScenePath, QOffscreenSurface* offscreenSurface)
{
    if(!sofaScene || !sofaScene->sofaSimulation() || sofaScenePath.isEmpty())
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

    GLenum err = glewInit();
    if(0 != err)
        qWarning() << "GLEW Initialization failed with error code:" << err;

    #ifdef __linux__
        static bool glutInited = false;
        if(!glutInited)
        {
            int argc = 0;
            glutInit(&argc, NULL);
            glutInited = true;
        }
    #endif

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

    if(sofaScene->sofaSimulation()->load(sofaScenePath.toLatin1().constData()))
    {
        if(!sofaScene->sofaSimulation()->GetRoot())
        {
            sofaScene->setStatus(SofaScene::Status::Error);
            return false;
        }

        sofaScene->sofaSimulation()->init(sofaScene->sofaSimulation()->GetRoot().get());
        sofaScene->sofaSimulation()->initTextures(sofaScene->sofaSimulation()->GetRoot().get());

		glFinish();

        sofaScene->addChild(0, sofaScene->sofaSimulation()->GetRoot().get());

        sofaScene->setStatus(SofaScene::Status::Ready);

        if(!sofaScene->myPathQML.isEmpty())
            sofaScene->setSourceQML(QUrl::fromLocalFile(sofaScene->myPathQML));

        return true;
    }

    sofaScene->setStatus(SofaScene::Status::Error);

	return false;
}

class LoaderThread : public QThread
{
public:
    LoaderThread(SofaScene* sofaScene, const QString& sofaScenePath, QOffscreenSurface* offscreenSurface) :
        mySofaScene(sofaScene),
        mySofaScenePath(sofaScenePath),
        myOffscreenSurface(offscreenSurface),
		myIsLoaded(false)
	{

	}

	void run()
	{
        myIsLoaded = LoaderProcess(mySofaScene, mySofaScenePath, myOffscreenSurface);
	}

	bool isLoaded() const			{return myIsLoaded;}

private:
    SofaScene*                      mySofaScene;
    QString							mySofaScenePath;
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
    myPathQML.clear();
    setSourceQML(QUrl());

    if(Status::Loading == myStatus) // return now if a SofaScene is already loading
        return;

    setPlay(false);

    setStatus(Status::Loading);

    aboutToUnload();

    setSelectedComponent(nullptr);
    setSelectedManipulator(nullptr);
    myManipulators.clear();

    mySofaSimulation->unload(mySofaSimulation->GetRoot());

	QString finalFilename = mySource.toLocalFile();
	if(finalFilename.isEmpty())
    {
		setStatus(Status::Error);
        return;
    }

    std::string filepath = finalFilename.toLatin1().constData();
    if(sofa::helper::system::DataRepository.findFile(filepath))
    {
        finalFilename = filepath.c_str();
        finalFilename.replace("\\", "/");
    }

    if(finalFilename.isEmpty())
    {
        setStatus(Status::Error);
        return;
    }

    bool currentAsynchronous = myAsynchronous;
    if(myPyQtSynchronous && currentAsynchronous)
    {
        QFile file(QString::fromStdString(filepath));
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            QString content = in.readAll();
            if(-1 != content.indexOf("PyQt", 0, Qt::CaseInsensitive)) {
                currentAsynchronous = false;
                qDebug() << "This scene seems to contain PyQt and will be loaded synchronously";
            }
        }
    }

    std::string qmlFilepath = (finalFilename + ".qml").toLatin1().constData();
    if(!sofa::helper::system::DataRepository.findFile(qmlFilepath))
        qmlFilepath.clear();

    myPathQML = QString::fromStdString(qmlFilepath);

    QString finalHeader;

    QFile baseHeaderFile(":/python/BaseHeader.py");
    if(!baseHeaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("ERROR: base header not found");
    }
    else
    {
        finalHeader += QTextStream(&baseHeaderFile).readAll();
    }

    finalHeader += myHeader;

    SceneLoaderPY::setHeader(finalHeader.toStdString());

    QOffscreenSurface* offScreenSurface = new QOffscreenSurface();
    offScreenSurface->create();

    if(currentAsynchronous)
	{
        LoaderThread* loaderThread = new LoaderThread(this, finalFilename, offScreenSurface);
		
        connect(loaderThread, &QThread::finished, this, [this, loaderThread, qmlFilepath, offScreenSurface]() {
            if(!loaderThread->isLoaded())
                setStatus(Status::Error);
            else
            {
                setDt(mySofaSimulation->GetRoot()->getDt());
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
        if(!LoaderProcess(this, finalFilename, offScreenSurface))
            setStatus(Status::Error);
        else
        {
            setDt(mySofaSimulation->GetRoot()->getDt());
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

	setStatus(Status::Null);

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

void SofaScene::setDt(double newDt)
{
	if(newDt == myDt)
		return;

	myDt = newDt;

	dtChanged(newDt);
}

void SofaScene::setPlay(bool newPlay)
{
	if(newPlay == myPlay)
		return;

	myPlay = newPlay;

	playChanged(newPlay);
}

void SofaScene::setAsynchronous(bool newAsynchronous)
{
    if(newAsynchronous == myAsynchronous)
        return;

    myAsynchronous = newAsynchronous;

    asynchronousChanged(newAsynchronous);
}

void SofaScene::setPyQtSynchronous(bool newPyQtSynchronous)
{
    if(newPyQtSynchronous == myPyQtSynchronous)
        return;

    myPyQtSynchronous = newPyQtSynchronous;

    pyQtSynchronousChanged(newPyQtSynchronous);
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
    mySofaSimulation->computeTotalBBox(mySofaSimulation->GetRoot().get(), pmin, pmax);

	min = QVector3D(pmin[0], pmin[1], pmin[2]);
	max = QVector3D(pmax[0], pmax[1], pmax[2]);
}

QString SofaScene::dumpGraph() const
{
	QString dump;

	if(mySofaSimulation->GetRoot())
	{
		std::streambuf* backup(std::cout.rdbuf());

		std::ostringstream stream;
		std::cout.rdbuf(stream.rdbuf());
		mySofaSimulation->print(mySofaSimulation->GetRoot().get());
		std::cout.rdbuf(backup);

		dump += QString::fromStdString(stream.str());
	}

	return dump;
}

bool SofaScene::reinitComponent(const QString& path)
{
    QStringList pathComponents = path.split("/");

    Node::SPtr node = mySofaSimulation->GetRoot();
    unsigned int i = 0;
    while(i+1 < (unsigned int)pathComponents.size()) {
        if (pathComponents[i]=="@") {
            ++i;
            continue;
        }

        node = node->getChild(pathComponents[i].toStdString());
        if (!node) {
            qWarning() << "Object path unknown:" << path;
            return false;
        }
        ++i;
    }
    BaseObject* object = node->get<BaseObject>(pathComponents[i].toStdString());
    if(!object) {
        qWarning() << "Object path unknown:" << path;
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
    if(!mySofaSimulation->GetRoot())
        return;

    sofa::core::objectmodel::GUIEvent event(controlID.toUtf8().constData(), valueName.toUtf8().constData(), value.toUtf8().constData());
    mySofaSimulation->GetRoot()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &event);
}

QVariantMap SofaScene::dataObject(const sofa::core::objectmodel::BaseData* data)
{
    QVariantMap object;

    if(!data)
    {
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
        properties.insert("decimals", 8);
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

    QString widget(data->getWidget());
    if(!widget.isEmpty())
        type = widget;

    properties.insert("readOnly", false);

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
                qWarning("Trying to set a list of values on a non-container data");
                return false;
            }

            if(valueIterable.size() != nbRows)
            {
                if(typeinfo->FixedSize())
                {
                    qWarning() << "The new data should have the same size, should be" << nbRows << ", got" << valueIterable.size();
                    return false;
                }

                typeinfo->setSize(data, valueIterable.size());
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
                            qWarning() << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
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
                            qWarning() << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
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
                            qWarning() << "The new sub data should have the same size, should be" << nbCols << ", got" << subValueIterable.size() << "- data size is:" << valueIterable.size();
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
            qWarning("Map type is not supported");
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
    return onDataValue(path);
}

void SofaScene::setDataValue(const QString& path, const QVariant& value)
{
    onSetDataValue(path, value);
}

static BaseData* FindDataHelper(BaseNode* node, const QString& path)
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

SofaData* SofaScene::data(const QString& path) const
{
    BaseData* data = FindDataHelper(mySofaSimulation->GetRoot().get(), path);
    if(!data)
        return 0;

    Base* base = data->getOwner();
    if(!base)
        return 0;

    return new SofaData(this, base, data);
}

SofaComponent* SofaScene::component(const QString& path) const
{
    BaseData* data = FindDataHelper(mySofaSimulation->GetRoot().get(), path + ".name"); // search for the "name" data of the component (this data is always present if the component exist)

    if(!data)
        return 0;

    Base* base = data->getOwner();
    if(!base)
        return 0;

    return new SofaComponent(this, base);
}

SofaComponent* SofaScene::visualStyleComponent() const
{
    if(mySofaSimulation->GetRoot())
    {
        sofa::component::visualmodel::VisualStyle* visualStyle = nullptr;

        mySofaSimulation->GetRoot()->get(visualStyle);
        if(visualStyle)
            return new SofaComponent(this, visualStyle);
    }

    return nullptr;
}

QVariant SofaScene::onDataValue(const QString& path) const
{
    BaseData* data = FindDataHelper(mySofaSimulation->GetRoot().get(), path);

    if(!data)
    {
        qWarning() << "DataPath unknown:" << path;
        return QVariant();
    }

    return dataValue(data);
}

void SofaScene::onSetDataValue(const QString& path, const QVariant& value)
{
    BaseData* data = FindDataHelper(mySofaSimulation->GetRoot().get(), path);

    if(!data)
    {
        qWarning() << "DataPath unknown:" << path;
    }
    else
    {
        if(!value.isNull())
        {
            QVariant finalValue = value;
            if(finalValue.userType() == qMetaTypeId<QJSValue>())
                finalValue = finalValue.value<QJSValue>().toVariant();

            // arguments from JS are packed in an array, we have to unpack it
            if(QVariant::List == finalValue.type())
            {
                QSequentialIterable valueIterable = finalValue.value<QSequentialIterable>();
                if(1 == valueIterable.size())
                    finalValue = valueIterable.at(0);
            }

            setDataValue(data, finalValue);
        }
    }
}

void SofaScene::reload()
{
    // TODO: ! NEED CURRENT OPENGL CONTEXT while releasing the old SofaScene
    //qDebug() << "reload - thread" << QThread::currentThread() << QOpenGLContext::currentContext() << (void*) &glLightfv;

    open();
}

void SofaScene::animate(bool play)
{
    if(!isReady())
        return;

    setPlay(play);
}

void SofaScene::step()
{
    if(!isReady())
		return;

	emit stepBegin();
    mySofaSimulation->animate(mySofaSimulation->GetRoot().get(), myDt);
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
    mySofaSimulation->reset(mySofaSimulation->GetRoot().get());
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
    {
        sofa::core::objectmodel::Base* base = sofaComponent->base();
        if(base)
            nodes.append(down_cast<Node>(base->toBaseNode()));
    }

    if(nodes.isEmpty())
        nodes.append(mySofaSimulation->GetRoot().get());

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
//        visualParams->sceneBBox() = mySofaSimulation->GetRoot()->f_bbox.getValue();
//        visualParams->setProjectionMatrix(_projmatrix);
//        visualParams->setModelViewMatrix(_mvmatrix);
    }

    if(myVisualDirty)
    {
        myVisualDirty = false;

        mySofaSimulation->updateVisual(mySofaSimulation->GetRoot().get());
    }

    for(sofa::simulation::Node* root : nodes)
    {
        if(!root)
            continue;

        mySofaSimulation->draw(sofa::core::visual::VisualParams::defaultInstance(), root);

        // draw normals
        if(viewer.drawNormals())
        {
            sofa::helper::vector<OglModel*> oglModels;
            root->getTreeObjects<OglModel>(&oglModels);

            for(OglModel* oglModel : oglModels)
            {
                const ResizableExtVector<ExtVec3fTypes::Coord>& vertices = oglModel->getVertices();
                const ResizableExtVector<ExtVec3fTypes::Deriv>& normals = oglModel->getVnormals();

                if(vertices.size() != normals.size())
                    continue;

                for(size_t i = 0; i < vertices.size(); ++i)
                {
                    ExtVec3fTypes::Coord vertex = vertices[i];
                    ExtVec3fTypes::Deriv normal = normals[i];
                    float length = qBound(0.0f, normal.norm(), 1.0f);

                    glColor3f(1.0f - length, length, length * length);
                    glBegin(GL_LINES);
                    {
                        glVertex3f(vertex.x(), vertex.y(), vertex.z());
                        glVertex3f(vertex.x() + normal.x() * viewer.normalsDrawLength(), vertex.y() + normal.y() * viewer.normalsDrawLength(), vertex.z() + normal.z() * viewer.normalsDrawLength());
                    }
                    glEnd();
                }
            }
        }
    }

    // highlight selected components using a specific shader
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

    glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

    // draw manipulators
    if(viewer.drawManipulators())
        for(Manipulator* manipulator : myManipulators)
            if(manipulator)
                manipulator->draw(viewer);
}

SelectableSofaParticle* SofaScene::pickParticle(const QVector3D& origin, const QVector3D& direction, double distanceToRay, double distanceToRayGrowth, const QList<SofaComponent*>& roots) const
{
    SelectableSofaParticle* selectableSofaParticle = nullptr;

    sofa::simulation::MechanicalPickParticlesVisitor pickVisitor(sofa::core::ExecParams::defaultInstance(),
                                                                 sofa::defaulttype::Vector3(origin.x(), origin.y(), origin.z()),
                                                                 sofa::defaulttype::Vector3(direction.x(), direction.y(), direction.z()),
                                                                 distanceToRay,
                                                                 distanceToRayGrowth);

    QList<sofa::simulation::Node*> nodes;
    nodes.reserve(roots.size());
    for(SofaComponent* sofaComponent : roots)
    {
        sofa::core::objectmodel::Base* base = sofaComponent->base();
        if(base)
            nodes.append(down_cast<Node>(base->toBaseNode()));
    }

    if(nodes.isEmpty())
        nodes.append(sofaSimulation()->GetRoot().get());

    for(sofa::simulation::Node* root : nodes)
    {
        if(!root)
            continue;

        pickVisitor.execute(root->getContext());

        if(!pickVisitor.particles.empty())
        {
            MechanicalObject3* mechanicalObject = dynamic_cast<MechanicalObject3*>(pickVisitor.particles.begin()->second.first);
            int index = pickVisitor.particles.begin()->second.second;

            if(mechanicalObject && -1 != index)
            {
                selectableSofaParticle = new SelectableSofaParticle(SofaComponent(this, mechanicalObject), index);
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

Selectable* SofaScene::pickObject(const SofaViewer& viewer, const QPointF& ssPoint, const QList<SofaComponent*>& roots)
{
    Selectable* selectable = nullptr;

    QList<sofa::simulation::Node*> nodes;
    nodes.reserve(roots.size());
    for(SofaComponent* sofaComponent : roots)
    {
        sofa::core::objectmodel::Base* base = sofaComponent->base();
        if(base)
            nodes.append(down_cast<Node>(base->toBaseNode()));
    }

    if(nodes.isEmpty())
        nodes.append(mySofaSimulation->GetRoot().get());

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
                nodes.append(sofaSimulation()->GetRoot().get());

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
                    if(visualModel)
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
                    if(triangleModel)
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

            if(viewer.drawManipulators())
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
            qWarning() << "SofaScene::pickObject(...) return an incorrect object index";
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
	sofaSimulation()->GetRoot()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}

void SofaScene::onKeyReleased(char key)
{
    if(!isReady())
        return;

	sofa::core::objectmodel::KeyreleasedEvent keyEvent(key);
	sofaSimulation()->GetRoot()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
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
