#ifndef SCENE_H
#define SCENE_H

#include "SofaQtQuickGUI.h"
#include "Manipulator.h"

#include <sofa/simulation/common/Simulation.h>
#include <sofa/simulation/common/MutationListener.h>

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

class Scene;

/// QtQuick wrapper for a Sofa component (i.e baseObject / baseNode), allowing us to share a component in a QML context
class SOFA_SOFAQTQUICKGUI_API SceneComponent : public QObject
{
    Q_OBJECT

public:
    SceneComponent(const Scene* scene, const sofa::core::objectmodel::Base* base);
    SceneComponent(const SceneComponent& sceneComponent);

public:
    Q_PROPERTY(QString name READ name)

public:

    QString name() const;
    sofa::core::objectmodel::Base* base();
    const sofa::core::objectmodel::Base* base() const;

    const Scene* scene() const;

private:
    const Scene*                                    myScene;
    mutable const sofa::core::objectmodel::Base*    myBase;

};

/// QtQuick wrapper for a Sofa base data, allowing us to share a component data in a QML context
class SOFA_SOFAQTQUICKGUI_API SceneData : public QObject
{
    Q_OBJECT

public:
    SceneData(const SceneComponent* sceneComponent, const sofa::core::objectmodel::BaseData* data);
    SceneData(const Scene* scene, const sofa::core::objectmodel::Base* base, const sofa::core::objectmodel::BaseData* data);
    SceneData(const SceneData& sceneData);

    Q_INVOKABLE QVariantMap object() const;

    Q_INVOKABLE QVariant value();
    Q_INVOKABLE bool setValue(const QVariant& value);
    Q_INVOKABLE bool setLink(const QString& path);

    sofa::core::objectmodel::BaseData* data();
    const sofa::core::objectmodel::BaseData* data() const;

private:
    const Scene*                                        myScene;
    mutable const sofa::core::objectmodel::Base*        myBase;
    mutable const sofa::core::objectmodel::BaseData*    myData;

};

class Viewer;

/// \class QtQuick wrapper for a Sofa scene, allowing us to simulate, modify and draw (basic function) a Sofa scene
class Scene : public QObject, private sofa::simulation::MutationListener
{
    Q_OBJECT

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
    Q_PROPERTY(double dt READ dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(bool play READ playing WRITE setPlay NOTIFY playChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(bool visualDirty READ visualDirty NOTIFY visualDirtyChanged)
    Q_PROPERTY(QQmlListProperty<sofa::qtquick::SceneComponent> selectedModels READ selectedModels DESIGNABLE false FINAL)
    Q_PROPERTY(QQmlListProperty<sofa::qtquick::Manipulator> selectedManipulators READ selectedManipulators DESIGNABLE false FINAL)
    Q_PROPERTY(QQmlListProperty<sofa::qtquick::Manipulator> manipulators READ manipulators DESIGNABLE false FINAL)

    Q_ENUMS(Status)
    enum Status {
		Null,
		Ready,
		Loading,
		Error
	};

public:
    sofa::qtquick::Scene::Status status()	const							{return myStatus;}
    void setStatus(sofa::qtquick::Scene::Status newStatus);

    bool isPreLoaded() const                        {return myIsInit;}
    bool isLoading() const							{return Status::Loading == myStatus;}
    bool isReady() const							{return Status::Ready == myStatus;}

    const QString& header() const					{return myHeader;}
    void setHeader(const QString& newHeader);

	const QUrl& source() const						{return mySource;}
	void setSource(const QUrl& newSource);

	const QUrl& sourceQML() const					{return mySourceQML;}
	void setSourceQML(const QUrl& newSourceQML);

	double dt() const								{return myDt;}
	void setDt(double newDt);
	
	bool playing() const							{return myPlay;}
	void setPlay(bool newPlay);

    bool asynchronous() const                       {return myAsynchronous;}
    void setAsynchronous(bool newPlay);

    bool visualDirty() const						{return myVisualDirty;}
    void setVisualDirty(bool newVisualDirty);

    QQmlListProperty<sofa::qtquick::Manipulator>    manipulators();
    QQmlListProperty<sofa::qtquick::Manipulator>    selectedManipulators();
    QQmlListProperty<sofa::qtquick::SceneComponent> selectedModels();

signals:
    void preloaded();                                   /// this signal is emitted after basic init has been done, call initGraphics() with a valid opengl context bound to effectively load the scene
    void loaded();                                      /// scene has been loaded and is ready
    void aboutToUnload();                               /// scene is being to be unloaded
    void statusChanged(Status newStatus);
    void headerChanged(const QString& newHeader);
	void sourceChanged(const QUrl& newSource);
	void sourceQMLChanged(const QUrl& newSourceQML);
	void dtChanged(double newDt);
	void playChanged(bool newPlay);
	void asynchronousChanged(bool newAsynchronous);
    void visualDirtyChanged(bool newVisualDirty);

public:
    Q_INVOKABLE double radius() const;
    Q_INVOKABLE void computeBoundingBox(QVector3D& min, QVector3D& max) const;
    Q_INVOKABLE QString dumpGraph() const;
    Q_INVOKABLE void reinitComponent(const QString& path);
    Q_INVOKABLE bool areSameComponent(SceneComponent* sceneComponentA, SceneComponent* sceneComponentB);
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
    void initGraphics();        // need an opengl context made current
	void reload();
    void animate(bool play);
	void step();
	void reset();

	void onKeyPressed(char key);
	void onKeyReleased(char key);

signals:
	void stepBegin();
    void stepEnd();
    void reseted();

private slots:
    void open();
    void handleStatusChange(Status newStatus);

public:
	sofa::simulation::Simulation* sofaSimulation() const {return mySofaSimulation;}

    /// \brief      Low-level drawing function
    /// \attention  Require an opengl context bound to a surface, viewport / projection / modelview must have been set
    /// \note       The best way to display a 'Scene' is to use a 'Viewer' instead of directly call this function
    void draw(const Viewer& viewer);

    /// \brief      Low-level function for color index picking
    /// \attention  Require an opengl context bound to a surface, viewport / projection / modelview must have been set
    /// \note       The best way to pick an object is to use a 'PickingInteractor' instead of directly call this function
    /// \return     True if an object has been picked, false if we hit the background or a non-selectable object
    bool pickUsingRasterization(const Viewer& viewer, const QPointF& nativePoint, SceneComponent*& sceneComponent, Manipulator*& manipulator, float& z);

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
    QString                                     myPathQML;
    bool                                        myIsInit;
    bool                                        myVisualDirty;
    double                                      myDt;
    bool                                        myPlay;
    bool                                        myAsynchronous;

    sofa::simulation::Simulation*               mySofaSimulation;
    QTimer*                                     myStepTimer;
    QSet<const sofa::core::objectmodel::Base*>  myBases;                        /// \todo For each base, reference a unique SceneComponent and use it in QML as a wrapper

    QList<Manipulator*>                         myManipulators;
    QList<Manipulator*>                         mySelectedManipulators;         /// \todo Currently we can select only one manipulator, change that
    QList<SceneComponent*>                      mySelectedModels;               /// \todo Currently we can select only one model, change that

    QOpenGLShaderProgram*                       mySelectedModelShaderProgram;
    QOpenGLShaderProgram*                       myPickingShaderProgram;
};

}

}

#endif // SCENE_H
