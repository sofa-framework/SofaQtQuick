#include <GL/glew.h>
#include "Scene.h"
#include "Viewer.h"
#include "SelectableManipulator.h"

#include <sofa/core/ObjectFactory.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/core/objectmodel/GUIEvent.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/PluginManager.h>
#include <sofa/simulation/graph/graph.h>
#include <sofa/simulation/graph/DAGSimulation.h>
#include <sofa/simulation/graph/init.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/DrawToolGL.h>
#include <sofa/helper/system/glut.h>
#include <SofaPython/SceneLoaderPY.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaMeshCollision/TriangleModel.h>
#include <SofaComponentMain/init.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/simulation/common/MechanicalVisitor.h>

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
using namespace sofa::component::collision;
using namespace sofa::simulation;

typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3dTypes> MechanicalObject3d;

Scene::Scene(QObject *parent) : QObject(parent), MutationListener(),
	myStatus(Status::Null),
	mySource(),
    mySourceQML(),
    myScreenshotFilename(),
    myPathQML(),
	myIsInit(false),
    myVisualDirty(false),
    myDrawNormals(false),
    myNormalsDrawLength(1.0f),
	myDt(0.04),
	myPlay(false),
	myAsynchronous(true),
    mySofaSimulation(nullptr),
    myStepTimer(new QTimer(this)),
    myBases(),
    myManipulators(),
    mySelectedManipulators(),
    mySelectedComponents(),
    myHighlightShaderProgram(nullptr),
    myPickingShaderProgram(nullptr)
{
    sofa::component::init();
    sofa::simulation::graph::init();

	sofa::core::ExecParams::defaultInstance()->setAspectID(0);
	boost::shared_ptr<sofa::core::ObjectFactory::ClassEntry> classVisualModel;
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
	connect(this, &Scene::sourceChanged, this, &Scene::open);
	connect(this, &Scene::playChanged, myStepTimer, [&](bool newPlay) {newPlay ? myStepTimer->start() : myStepTimer->stop();});
    connect(this, &Scene::statusChanged, this, &Scene::handleStatusChange);
    connect(this, &Scene::loaded, this, [&]() {addChild(0, mySofaSimulation->GetRoot().get());});
    connect(this, &Scene::aboutToUnload, this, [&]() {myBases.clear();});

    connect(myStepTimer, &QTimer::timeout, this, &Scene::step);
}

Scene::~Scene()
{
	if(mySofaSimulation == sofa::simulation::getSimulation())
		sofa::simulation::setSimulation(0);

    sofa::simulation::graph::cleanup();
}

static bool LoaderProcess(sofa::simulation::Simulation* sofaSimulation, const QString& scenePath)
{
	if(!sofaSimulation || scenePath.isEmpty())
		return false;

	sofa::core::visual::VisualParams* vparams = sofa::core::visual::VisualParams::defaultInstance();
	if(vparams)
		vparams->displayFlags().setShowVisualModels(true);

    if(sofaSimulation->load(scenePath.toLatin1().constData()))
        if(sofaSimulation->GetRoot()) {
            sofaSimulation->init(sofaSimulation->GetRoot().get());
			return true;
        }

	return false;
}

class LoaderThread : public QThread
{
public:
    LoaderThread(sofa::simulation::Simulation* sofaSimulation, const QString& scenePath) :
		mySofaSimulation(sofaSimulation),
		myScenepath(scenePath),
		myIsLoaded(false)
	{

	}

	void run()
	{
        myIsLoaded = LoaderProcess(mySofaSimulation, myScenepath);
	}

	bool isLoaded() const			{return myIsLoaded;}

private:
	sofa::simulation::Simulation*	mySofaSimulation;
	QString							myScenepath;
	bool							myIsLoaded;

};

void Scene::open()
{
    myPathQML.clear();
	setSourceQML(QUrl());

	if(Status::Loading == myStatus) // return now if a scene is already loading
		return;

	QString finalFilename = mySource.toLocalFile();
	if(finalFilename.isEmpty())
	{
		setStatus(Status::Error);
		return;
	}

	std::string filepath = finalFilename.toLatin1().constData();
	if(sofa::helper::system::DataRepository.findFile(filepath))
		finalFilename = filepath.c_str();

	if(finalFilename.isEmpty())
	{
		setStatus(Status::Error);
		return;
	}

	finalFilename.replace("\\", "/");

    aboutToUnload();

	setStatus(Status::Loading);

	setPlay(false);
	myIsInit = false;

    std::string qmlFilepath = (finalFilename + ".qml").toLatin1().constData();
    if(!sofa::helper::system::DataRepository.findFile(qmlFilepath))
        qmlFilepath.clear();

    myPathQML = QString::fromStdString(qmlFilepath);

    mySofaSimulation->unload(mySofaSimulation->GetRoot());

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

	if(myAsynchronous)
	{
        LoaderThread* loaderThread = new LoaderThread(mySofaSimulation, finalFilename);

        connect(loaderThread, &QThread::finished, this, [this, loaderThread, qmlFilepath]() {                    
            if(!loaderThread->isLoaded())
                setStatus(Status::Error);
            else
            {
                myIsInit = true;
                preloaded();
            }

            loaderThread->deleteLater();
        });

		loaderThread->start();
	}
    else
	{
        if(!LoaderProcess(mySofaSimulation, finalFilename))
            setStatus(Status::Error);
        else
        {
            myIsInit = true;
            preloaded();
        }
	}
}

void Scene::handleStatusChange(Scene::Status newStatus)
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
        qCritical() << "Scene status unknown";
        break;
    }
}

void Scene::takeScreenshot()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);
    screenshot.init(viewport[2], viewport[3], 1, 1, helper::io::Image::UNORM8, helper::io::Image::RGB);
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], GL_RGB, GL_UNSIGNED_BYTE, screenshot.getPixels());
}

void Scene::saveScreenshotInFile()
{
    QString finalFilename = myScreenshotFilename.toLocalFile();
    if(finalFilename.isEmpty())
    {
        std::cerr << "File to save screenshot doesn't exist" << std::endl;
        return;
    }

    std::string filepath = finalFilename.toLatin1().constData();

    if (!screenshot.save(filepath)) return;
        std::cout << "Saved "<<screenshot.getWidth()<<"x"<<screenshot.getHeight()<<" screen image to "<<filepath<<std::endl;
    glReadBuffer(GL_BACK);
}

void Scene::setStatus(Status newStatus)
{
	if(newStatus == myStatus)
		return;

	myStatus = newStatus;

	statusChanged(newStatus);
}

void Scene::setHeader(const QString& newHeader)
{
    if(newHeader == myHeader || Status::Loading == myStatus)
        return;

    myHeader = newHeader;

    headerChanged(newHeader);
}

void Scene::setSource(const QUrl& newSource)
{
	if(newSource == mySource || Status::Loading == myStatus)
		return;

	setStatus(Status::Null);

	mySource = newSource;

	sourceChanged(newSource);
}

void Scene::setSourceQML(const QUrl& newSourceQML)
{
	if(newSourceQML == mySourceQML)
		return;

	mySourceQML = newSourceQML;

	sourceQMLChanged(newSourceQML);
}

void Scene::setScreenshotFilename(const QUrl& newScreenshotFilename)
{
    if(newScreenshotFilename == myScreenshotFilename)
        return;

    myScreenshotFilename = newScreenshotFilename;

    screenshotFilenameChanged(newScreenshotFilename);
}

void Scene::setDt(double newDt)
{
	if(newDt == myDt)
		return;

	myDt = newDt;

	dtChanged(newDt);
}

void Scene::setPlay(bool newPlay)
{
	if(newPlay == myPlay)
		return;

	myPlay = newPlay;

	playChanged(newPlay);
}

void Scene::setAsynchronous(bool newAsynchronous)
{
    if(newAsynchronous == myAsynchronous)
        return;

    myAsynchronous = newAsynchronous;

    asynchronousChanged(newAsynchronous);
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

QQmlListProperty<sofa::qtquick::Manipulator> Scene::manipulators()
{
    return QQmlListProperty<sofa::qtquick::Manipulator>(this, &myManipulators, appendManipulator, countManipulator, atManipulator, clearManipulator);
}

QQmlListProperty<sofa::qtquick::Manipulator> Scene::selectedManipulators()
{
    return QQmlListProperty<sofa::qtquick::Manipulator>(this, &mySelectedManipulators, appendManipulator, countManipulator, atManipulator, clearManipulator);
}

static void appendSelectedComponent(QQmlListProperty<sofa::qtquick::SceneComponent> *property, sofa::qtquick::SceneComponent *value)
{
    // TODO: is it really useful ?
    SceneComponent* valueCopy = new SceneComponent(*value);

    static_cast<QList<sofa::qtquick::SceneComponent*>*>(property->data)->append(valueCopy);

    qobject_cast<Scene*>(property->object)->selectedComponentsChanged();
}

static sofa::qtquick::SceneComponent* atSelectedComponent(QQmlListProperty<sofa::qtquick::SceneComponent> *property, int index)
{
    return static_cast<QList<sofa::qtquick::SceneComponent*>*>(property->data)->at(index);
}

static void clearSelectedComponent(QQmlListProperty<sofa::qtquick::SceneComponent> *property)
{
    QList<sofa::qtquick::SceneComponent*>& selectedComponents = *static_cast<QList<sofa::qtquick::SceneComponent*>*>(property->data);
    for(SceneComponent* sceneComponent : selectedComponents)
        delete sceneComponent;

    static_cast<QList<sofa::qtquick::SceneComponent*>*>(property->data)->clear();

    qobject_cast<Scene*>(property->object)->selectedComponentsChanged();
}

static int countSelectedComponent(QQmlListProperty<sofa::qtquick::SceneComponent> *property)
{
    return static_cast<QList<sofa::qtquick::SceneComponent*>*>(property->data)->size();
}

QQmlListProperty<sofa::qtquick::SceneComponent> Scene::selectedComponents()
{
    return QQmlListProperty<sofa::qtquick::SceneComponent>(this, &mySelectedComponents, appendSelectedComponent, countSelectedComponent, atSelectedComponent, clearSelectedComponent);
}

double Scene::radius() const
{
	QVector3D min, max;
	computeBoundingBox(min, max);
	QVector3D diag = (max - min);

	return diag.length();
}

void Scene::computeBoundingBox(QVector3D& min, QVector3D& max) const
{
	SReal pmin[3], pmax[3];
    mySofaSimulation->computeTotalBBox(mySofaSimulation->GetRoot().get(), pmin, pmax);

	min = QVector3D(pmin[0], pmin[1], pmin[2]);
	max = QVector3D(pmax[0], pmax[1], pmax[2]);
}

QString Scene::dumpGraph() const
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

void Scene::reinitComponent(const QString& path)
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
            return;
        }
        ++i;
    }
    BaseObject* object = node->get<BaseObject>(pathComponents[i].toStdString());
    if(!object) {
        qWarning() << "Object path unknown:" << path;
        return;
    }
    object->reinit();
}

bool Scene::areSameComponent(SceneComponent* sceneComponentA, SceneComponent* sceneComponentB)
{
    if(!sceneComponentA)
        return false;

    return sceneComponentA->isSame(sceneComponentB);
}

void Scene::sendGUIEvent(const QString& controlID, const QString& valueName, const QString& value)
{
    if(!mySofaSimulation->GetRoot())
        return;

    sofa::core::objectmodel::GUIEvent event(controlID.toUtf8().constData(), valueName.toUtf8().constData(), value.toUtf8().constData());
    mySofaSimulation->GetRoot()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &event);
}

QVariantMap Scene::dataObject(const sofa::core::objectmodel::BaseData* data)
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
        properties.insert("decimals", 3);
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

QVariant Scene::dataValue(const BaseData* data)
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
bool Scene::setDataValue(BaseData* data, const QVariant& value)
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

bool Scene::setDataLink(BaseData* data, const QString& link)
{
    if(!data)
        return false;

    if(link.isEmpty())
        data->setParent(0);
    else
        data->setParent(link.toStdString());

    return data->getParent();
}

QVariant Scene::dataValue(const QString& path) const
{
    return onDataValue(path);
}

void Scene::setDataValue(const QString& path, const QVariant& value)
{
    onSetDataValue(path, value);
}

static BaseData* FindDataHelper(BaseNode* node, const QString& path)
{
    BaseData* data = 0;
    std::streambuf* backup(std::cerr.rdbuf());

    std::ostringstream stream;
    std::cerr.rdbuf(stream.rdbuf());
    node->findDataLinkDest(data, path.toStdString(), 0);
    std::cerr.rdbuf(backup);

    return data;
}

SceneData* Scene::data(const QString& path) const
{
    BaseData* data = FindDataHelper(mySofaSimulation->GetRoot().get(), path);
    if(!data)
        return 0;

    Base* base = data->getOwner();
    if(!base)
        return 0;

    return new SceneData(this, base, data);
}

SceneComponent* Scene::component(const QString& path) const
{
    BaseData* data = FindDataHelper(mySofaSimulation->GetRoot().get(), path + ".name"); // search for the "name" data of the component (this data is always present if the component exist)

    if(!data)
        return 0;

    Base* base = data->getOwner();
    if(!base)
        return 0;

    return new SceneComponent(this, base);
}

QVariant Scene::onDataValue(const QString& path) const
{
    BaseData* data = FindDataHelper(mySofaSimulation->GetRoot().get(), path);

    if(!data)
    {
        qWarning() << "DataPath unknown:" << path;
        return QVariant();
    }

    return dataValue(data);
}

void Scene::onSetDataValue(const QString& path, const QVariant& value)
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

void Scene::initGraphics()
{
    if(!isPreLoaded() || !isLoading())
        return;

    if(!mySofaSimulation->GetRoot())
    {
        setStatus(Status::Error);
		return;
    }

    GLenum err = glewInit();
    if(0 != err)
        qWarning() << "GLEW Initialization failed with error code:" << err;

    // init the highlight shader program
    if(!myHighlightShaderProgram)
    {
        myHighlightShaderProgram = new QOpenGLShaderProgram(this);

        myHighlightShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            "void main(void)\n"
            "{\n"
            "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}");
        myHighlightShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            "void main(void)\n"
            "{\n"
            "   gl_FragColor = vec4(0.75, 0.5, 0.0, 1.0);\n"
            "}");

        myHighlightShaderProgram->link();
    }

    // init the picking shader program
    if(!myPickingShaderProgram)
    {
        myPickingShaderProgram = new QOpenGLShaderProgram();
        myPickingShaderProgram->create();
        myPickingShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            "void main(void)\n"
            "{\n"
            "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}");
        myPickingShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            "uniform highp vec4 index;\n"
            "void main(void)\n"
            "{\n"
            "   gl_FragColor = index;\n"
            "}");
        myPickingShaderProgram->link();
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
    }

#ifdef __linux__
    static bool glutInited = false;
    if(!glutInited)
    {
        int argc = 0;
        glutInit(&argc, NULL);
        glutInited = true;
    }
#endif

    // need a valig OpenGL context for initTextures
	mySofaSimulation->initTextures(mySofaSimulation->GetRoot().get());
	setDt(mySofaSimulation->GetRoot()->getDt());

    setStatus(Status::Ready);

    if(!myPathQML.isEmpty())
        setSourceQML(QUrl::fromLocalFile(myPathQML));
}

void Scene::reload()
{
    // TODO: ! NEED CURRENT OPENGL CONTEXT while releasing the old sofa scene
    //qDebug() << "reload - thread" << QThread::currentThread() << QOpenGLContext::currentContext() << (void*) &glLightfv;

    open();
}

void Scene::animate(bool play)
{
    if(!isReady())
        return;

    setPlay(play);
}

void Scene::step()
{
    if(!isReady())
		return;

	emit stepBegin();
    mySofaSimulation->animate(mySofaSimulation->GetRoot().get(), myDt);
    myVisualDirty = true;
    emit stepEnd();
}

void Scene::reset()
{
    if(!isReady())
        return;

    // TODO: ! NEED CURRENT OPENGL CONTEXT
    mySofaSimulation->reset(mySofaSimulation->GetRoot().get());
    myVisualDirty = true;
    emit reseted();
}

void Scene::draw(const Viewer& viewer) const
{
    if(!isReady())
        return;

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
        mySofaSimulation->updateVisual(mySofaSimulation->GetRoot().get());
        myVisualDirty = false;
    }

    mySofaSimulation->draw(sofa::core::visual::VisualParams::defaultInstance(), mySofaSimulation->GetRoot().get());

    // draw normals
    if(myDrawNormals)
    {
        Node* root = sofaSimulation()->GetRoot().get();

        sofa::helper::vector<OglModel*> oglModels;
        root->getTreeObjects<OglModel>(&oglModels);

        for(OglModel* oglModel : oglModels)
        {
            const ResizableExtVector<ExtVec3fTypes::Coord>& vertices = oglModel->getVertices();
            const ResizableExtVector<ExtVec3fTypes::Deriv>& normals = oglModel->getVnormals();

            if(vertices.size() != normals.size())
                continue;

            for(int i = 0; i < vertices.size(); ++i)
            {
                ExtVec3fTypes::Coord vertex = vertices[i];
                ExtVec3fTypes::Deriv normal = normals[i];
                float length = qBound(0.0f, normal.norm(), 1.0f);

                glColor3f(1.0f - length, length, length * length);
                glBegin(GL_LINES);
                {
                    glVertex3f(vertex.x(), vertex.y(), vertex.z());
                    glVertex3f(vertex.x() + normal.x() * myNormalsDrawLength, vertex.y() + normal.y() * myNormalsDrawLength, vertex.z() + normal.z() * myNormalsDrawLength);
                }
                glEnd();
            }
        }
    }

    // highlight selected components using a specific shader
    if(!mySelectedComponents.isEmpty())
    {
        glDepthFunc(GL_ALWAYS);
        glDepthMask(GL_FALSE);

        myHighlightShaderProgram->bind();
        for(SceneComponent* sceneComponent : mySelectedComponents)
        {
            if(sceneComponent)
            {
                OglModel* oglModel = dynamic_cast<OglModel*>(sceneComponent->base());
                if(oglModel)
                {
                    VisualStyle* visualStyle = nullptr;
                    oglModel->getContext()->get(visualStyle);

                    if(visualStyle)
                        visualStyle->fwdDraw(visualParams);

                    visualParams->displayFlags().setShowWireFrame(true);

                    oglModel->drawVisual(visualParams);

                    if(visualStyle)
                        visualStyle->bwdDraw(visualParams);
                }
                else
                {
                    TriangleModel* triangleModel = dynamic_cast<TriangleModel*>(sceneComponent->base());
                    if(triangleModel)
                    {
                        VisualStyle* visualStyle = nullptr;
                        triangleModel->getContext()->get(visualStyle);

                        if(visualStyle)
                            visualStyle->fwdDraw(visualParams);

                        visualParams->displayFlags().setShowWireFrame(true);

                        triangleModel->draw(visualParams);

                        if(visualStyle)
                            visualStyle->bwdDraw(visualParams);
                    }
                }
            }
        }
        myHighlightShaderProgram->release();

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glDisable(GL_POLYGON_OFFSET_LINE);
    }

    glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);

    if(!myManipulators.isEmpty())
    {
        // since manipulator may use transparency, we order them by z
        QVector<Manipulator*> zOrderedManipulators(QVector<Manipulator*>::fromList(myManipulators));
        qSort(zOrderedManipulators.begin(), zOrderedManipulators.end(), [&](Manipulator* a, Manipulator* b) {
            return viewer.computeDepth(a->position()) < viewer.computeDepth(b->position());
        });

        for(Manipulator* zOrderedManipulator : zOrderedManipulators)
            if(zOrderedManipulator)
                zOrderedManipulator->draw(viewer);
    }
}

SelectableSceneParticle* Scene::pickParticle(const QVector3D& origin, const QVector3D& direction, double distanceToRay, double distanceToRayGrowth) const
{
    SelectableSceneParticle* selectableSceneParticle = nullptr;

    sofa::simulation::MechanicalPickParticlesVisitor pickVisitor(sofa::core::ExecParams::defaultInstance(),
                                                                 sofa::defaulttype::Vector3(origin.x(), origin.y(), origin.z()),
                                                                 sofa::defaulttype::Vector3(direction.x(), direction.y(), direction.z()),
                                                                 distanceToRay,
                                                                 distanceToRayGrowth);

    pickVisitor.execute(sofaSimulation()->GetRoot()->getContext());

    if(!pickVisitor.particles.empty())
    {
        MechanicalObject3d* mechanicalObject = dynamic_cast<MechanicalObject3d*>(pickVisitor.particles.begin()->second.first);
        int index = pickVisitor.particles.begin()->second.second;

        if(mechanicalObject && -1 != index)
            selectableSceneParticle = new SelectableSceneParticle(SceneComponent(this, mechanicalObject), index);
    }

    return selectableSceneParticle;
}

static QVector4D packPickingIndex(int i)
{
    return QVector4D((i & 0x000000FF) / 255.0, ((i & 0x0000FF00) >> 8) / 255.0, ((i & 0x00FF0000) >> 16) / 255.0, 0.0);
}

static int unpackPickingIndex(const std::array<unsigned char, 4>& i)
{
    return (i[0] | (i[1] << 8) | (i[2] << 16)) - 1;
}

Selectable* Scene::pickObject(const Viewer& viewer, const QPointF& nativePoint)
{
    Node* root = sofaSimulation()->GetRoot().get();

    sofa::helper::vector<OglModel*> oglModels;
    root->getTreeObjects<OglModel>(&oglModels);

    sofa::helper::vector<TriangleModel*> triangleModels;
    root->getTreeObjects<TriangleModel>(&triangleModels);

    if(!oglModels.empty() || !triangleModels.empty() || !myManipulators.empty())
    {
        sofa::core::visual::VisualParams* visualParams = sofa::core::visual::VisualParams::defaultInstance();

        myPickingShaderProgram->bind();
        {
            int indexLocation = myPickingShaderProgram->uniformLocation("index");

            unsigned int j = 0;
            for(; j < oglModels.size(); ++j)
            {
                OglModel* oglModel = oglModels[j];
                if(oglModel)
                {
                    myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(j + 1));
                    oglModel->drawVisual(visualParams);
                }
            }

            for(; j < triangleModels.size(); ++j)
            {
                TriangleModel* triangleModel = triangleModels[j];
                if(triangleModel)
                {
                    VisualStyle* visualStyle = nullptr;
                    triangleModel->getContext()->get(visualStyle);

                    myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(j + 1));

                    if(visualStyle)
                        visualStyle->fwdDraw(visualParams);

                    triangleModel->draw(visualParams);

                    if(visualStyle)
                        visualStyle->bwdDraw(visualParams);
                }
            }

            for(Manipulator* manipulator : myManipulators)
            {
                if(manipulator)
                {
                    myPickingShaderProgram->setUniformValue(indexLocation, packPickingIndex(j + 1));
                    manipulator->pick(viewer);
                }

                ++j;
            }
        }
        myPickingShaderProgram->release();

        std::array<unsigned char, 4> indexComponents;
        glReadPixels(nativePoint.x(), nativePoint.y(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, indexComponents.data());

        int j = unpackPickingIndex(indexComponents);
        if(-1 != j)
        {
            if(j < oglModels.size())
                return new SelectableSceneComponent(SceneComponent(this, oglModels[j]));

            j -= oglModels.size();
            if(j < triangleModels.size())
                return new SelectableSceneComponent(SceneComponent(this, triangleModels[j]));

            j -= triangleModels.size();
            if(j < myManipulators.size())
                return new SelectableManipulator(*myManipulators[j]);
        }
    }

    return nullptr;
}

void Scene::onKeyPressed(char key)
{
    if(!isReady())
        return;

	sofa::core::objectmodel::KeypressedEvent keyEvent(key);
	sofaSimulation()->GetRoot()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}

void Scene::onKeyReleased(char key)
{
    if(!isReady())
        return;

	sofa::core::objectmodel::KeyreleasedEvent keyEvent(key);
	sofaSimulation()->GetRoot()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &keyEvent);
}

void Scene::addChild(Node* parent, Node* child)
{
    if(!child)
        return;

    myBases.insert(child);

    MutationListener::addChild(parent, child);
}

void Scene::removeChild(Node* parent, Node* child)
{
    if(!child)
        return;

    MutationListener::removeChild(parent, child);

    myBases.remove(child);
}

void Scene::addObject(Node* parent, BaseObject* object)
{
    if(!object || !parent)
        return;

    myBases.insert(object);

    MutationListener::addObject(parent, object);
}

void Scene::removeObject(Node* parent, BaseObject* object)
{
    if(!object || !parent)
        return;

    MutationListener::removeObject(parent, object);

    myBases.remove(object);
}

}

}
