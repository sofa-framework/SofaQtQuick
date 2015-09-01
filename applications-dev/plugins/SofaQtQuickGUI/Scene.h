#ifndef SCENE_H
#define SCENE_H

#include "SofaQtQuickGUI.h"
#include "SceneComponent.h"
#include "SceneData.h"
#include "Manipulator.h"
#include "SelectableSceneParticle.h"

#include <sofa/simulation/common/Simulation.h>
#include <sofa/simulation/common/MutationListener.h>
#include <sofa/helper/io/ImageBMP.h>
#ifdef SOFA_HAVE_PNG
#include <sofa/helper/io/ImagePNG.h>
#endif

#include <QObject>
#include <QQmlListProperty>
#include <QVariant>
#include <QSet>
#include <QVector3D>
#include <QUrl>

class QTimer;
class QOpenGLShaderProgram;

namespace sofa
{

namespace qtquick
{

class Viewer;
class InitGraphicsWorker;
class PickUsingRasterizationWorker;

/// \class QtQuick wrapper for a Sofa scene, allowing us to simulate, modify and draw (basic function) a Sofa scene
class Scene : public QObject, private sofa::simulation::MutationListener
{
    Q_OBJECT

    friend class Viewer;
    friend class InitGraphicsWorker;
    friend class PickUsingRasterizationWorker;
    friend class SceneComponent;
    friend class SceneData;

public:
    explicit Scene(QObject *parent = 0);
	~Scene();

public:
    Q_PROPERTY(sofa::qtquick::Scene::Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl sourceQML READ sourceQML WRITE setSourceQML NOTIFY sourceQMLChanged)
    Q_PROPERTY(QUrl screenshotFilename READ screenshotFilename WRITE setScreenshotFilename NOTIFY screenshotFilenameChanged)
    Q_PROPERTY(double dt READ dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(bool play READ playing WRITE setPlay NOTIFY playChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(sofa::qtquick::SceneComponent* selectedComponent READ selectedComponent WRITE setSelectedComponent NOTIFY selectedComponentChanged)
    Q_PROPERTY(sofa::qtquick::Manipulator* selectedManipulator READ selectedManipulator WRITE setSelectedManipulator NOTIFY selectedManipulatorChanged)
    Q_PROPERTY(QQmlListProperty<sofa::qtquick::Manipulator> manipulators READ manipulators DESIGNABLE false FINAL)

    Q_ENUMS(Status)
    enum Status {
		Null,
		Ready,
		Loading,
		Error
	};

public:
    sofa::qtquick::Scene::Status status()	const               {return myStatus;}
    void setStatus(sofa::qtquick::Scene::Status newStatus);

    bool isLoading() const                                      {return Status::Loading == myStatus;}
    bool isReady() const                                        {return Status::Ready == myStatus;}

    const QString& header() const                               {return myHeader;}
    void setHeader(const QString& newHeader);

    const QUrl& source() const                                  {return mySource;}
	void setSource(const QUrl& newSource);

    const QUrl& sourceQML() const                               {return mySourceQML;}
	void setSourceQML(const QUrl& newSourceQML);

    const QUrl& screenshotFilename() const                      {return myScreenshotFilename;}
    void setScreenshotFilename(const QUrl& newScreenshotFilename);

    double dt() const                                           {return myDt;}
	void setDt(double newDt);
	
    bool playing() const                                        {return myPlay;}
	void setPlay(bool newPlay);

    bool asynchronous() const                                   {return myAsynchronous;}
    void setAsynchronous(bool newPlay);

    sofa::qtquick::SceneComponent* selectedComponent() const    {return mySelectedComponent;}
    void setSelectedComponent(sofa::qtquick::SceneComponent* newSelectedComponent);

    sofa::qtquick::Manipulator* selectedManipulator() const     {return mySelectedManipulator;}
    void setSelectedManipulator(sofa::qtquick::Manipulator* newSelectedManipulator);

    bool drawNormals() const                                    {return myDrawNormals;}
    void setDrawNormals(bool newDrawNormals)                    {myDrawNormals = newDrawNormals;}

    float normalsDrawLength() const                             {return myNormalsDrawLength;}
    void setNormalsDrawLength(float newNormalDrawLength)        {myNormalsDrawLength = newNormalDrawLength;}

    QQmlListProperty<sofa::qtquick::Manipulator>    manipulators();

signals:
    void loaded();                                      /// scene has been loaded and is ready
    void aboutToUnload();                               /// scene is being to be unloaded
    void statusChanged(Status newStatus);
    void headerChanged(const QString& newHeader);
	void sourceChanged(const QUrl& newSource);
	void sourceQMLChanged(const QUrl& newSourceQML);
    void screenshotFilenameChanged(const QUrl& newScreenshotFilename);
	void dtChanged(double newDt);
	void playChanged(bool newPlay);
    void asynchronousChanged(bool newAsynchronous);
    void selectedComponentChanged(sofa::qtquick::SceneComponent* newSelectedComponent);
    void selectedManipulatorChanged(sofa::qtquick::Manipulator* newSelectedManipulator);

public:
    Q_INVOKABLE double radius() const;
    Q_INVOKABLE void computeBoundingBox(QVector3D& min, QVector3D& max) const;
    Q_INVOKABLE QString dumpGraph() const;
    Q_INVOKABLE void reinitComponent(const QString& path);
    Q_INVOKABLE bool areSameComponent(SceneComponent* sceneComponentA, SceneComponent* sceneComponentB);
    Q_INVOKABLE bool areInSameBranch(SceneComponent* sceneComponentA, SceneComponent* sceneComponentB);
    Q_INVOKABLE void sendGUIEvent(const QString& controlID, const QString& valueName, const QString& value);

public:
    static QVariantMap dataObject(const sofa::core::objectmodel::BaseData* data);
    static QVariant dataValue(const sofa::core::objectmodel::BaseData* data);
    static bool setDataValue(sofa::core::objectmodel::BaseData* data, const QVariant& value);
    static bool setDataLink(sofa::core::objectmodel::BaseData* data, const QString& link);

    QVariant dataValue(const QString& path) const;
    void setDataValue(const QString& path, const QVariant& value);

    Q_INVOKABLE sofa::qtquick::SceneData* data(const QString& path) const;
    Q_INVOKABLE sofa::qtquick::SceneComponent* component(const QString& path) const;

protected:
    Q_INVOKABLE QVariant onDataValue(const QString& path) const;
    Q_INVOKABLE void onSetDataValue(const QString& path, const QVariant& value);

public slots:
	void reload();
    void animate(bool play);
	void step();
	void reset();
    void takeScreenshot();
    void saveScreenshotInFile();

	void onKeyPressed(char key);
	void onKeyReleased(char key);

signals:
	void stepBegin();
    void stepEnd();
    void reseted();

private slots:
    void open();
    void initGraphics();
    void handleStatusChange(Status newStatus);

public:
	sofa::simulation::Simulation* sofaSimulation() const {return mySofaSimulation;}

protected:
    /// \brief      Low-level drawing function
    /// \attention  Require an opengl context bound to a surface, viewport / projection / modelview must have been set
    /// \note       The best way to display a 'Scene' is to use a 'Viewer' instead of directly call this function
    void draw(const Viewer& viewer) const;

    SelectableSceneParticle*  pickParticle(const QVector3D& origin, const QVector3D& direction, double distanceToRay, double distanceToRayGrowth) const;

    /// \brief      Low-level function for color index picking
    /// \attention  Require an opengl context bound to a surface, viewport / projection / modelview must have been set
    /// \note       The best way to pick an object is to use a Viewer instead of directly call this function
    /// \return     True if an object has been picked, false if we hit the background or a non-selectable object
    Selectable* pickObject(const Viewer& viewer, const QPointF& nativePoint);

protected:
    void addChild(sofa::simulation::Node* parent, sofa::simulation::Node* child);
    void removeChild(sofa::simulation::Node* parent, sofa::simulation::Node* child);
    void addObject(sofa::simulation::Node* parent, sofa::core::objectmodel::BaseObject* object);
    void removeObject(sofa::simulation::Node* parent, sofa::core::objectmodel::BaseObject* object);

private:
    Status                                      myStatus;
    QString                                     myHeader;
    QUrl                                        mySource;
    QUrl                                        mySourceQML;
    QUrl                                        myScreenshotFilename;
    QString                                     myPathQML;
    mutable bool                                myVisualDirty;
    bool                                        myDrawNormals;
    float                                       myNormalsDrawLength;
    double                                      myDt;
    bool                                        myPlay;
    bool                                        myAsynchronous;

    sofa::simulation::Simulation*               mySofaSimulation;
    QTimer*                                     myStepTimer;
    QSet<const sofa::core::objectmodel::Base*>  myBases;                        /// \todo For each base, reference a unique SceneComponent and use it in QML as a wrapper

    QList<Manipulator*>                         myManipulators;
    Manipulator*                                mySelectedManipulator;
    SceneComponent*                             mySelectedComponent;

    QOpenGLShaderProgram*                       myHighlightShaderProgram;
    QOpenGLShaderProgram*                       myPickingShaderProgram;

    #ifdef SOFA_HAVE_PNG
        sofa::helper::io::ImagePNG screenshot;
    #else
        sofa::helper::io::ImageBMP screenshot;
    #endif
};

}

}

#endif // SCENE_H
