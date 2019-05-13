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
#pragma once

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Bindings/SofaNode.h>

#include <SofaQtQuickGUI/SofaComponentList.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>
#include <SofaQtQuickGUI/Bindings/SofaLink.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator.h>
#include <SofaQtQuickGUI/SelectableSofaParticle.h>

#include <sofa/simulation/Simulation.h>
#include <sofa/simulation/MutationListener.h>

#include <QObject>
#include <QQmlListProperty>
#include <QVariant>
#include <QSet>
#include <QVector3D>
#include <QUrl>
#include <QImage>

class QTimer;
class QOpenGLShaderProgram;
class QOpenGLFramebufferObject;
class QOffscreenSurface;

namespace sofa {
    namespace simulation {
        namespace _scenechecking_ {
            class SceneCheckerVisitor ;
        }
        namespace scenechecking {
            using _scenechecking_::SceneCheckerVisitor ;
        }
    }
}

namespace sofa
{

namespace simulation
{
    class Node;
}

namespace qtquick
{

class SofaScene;
class SofaViewer;
class PickUsingRasterizationWorker;
using sofaqtquick::bindings::SofaBase;
using sofaqtquick::bindings::SofaNode;

typedef QList<QUrl> QUrlList;

/// \class QtQuick wrapper for a Sofa scene, allowing us to simulate,
/// modify and draw (basic function) a Sofa scene
class SOFA_SOFAQTQUICKGUI_API SofaScene : public QObject
{
    Q_OBJECT

    friend class SofaViewer;
    friend class CameraView;
    friend class EditView;

    friend class PickUsingRasterizationWorker;
    friend bool LoaderProcess(SofaScene* scene);

public:
    explicit SofaScene(QObject *parent = nullptr);
    ~SofaScene();

public:
    Q_PROPERTY(sofa::qtquick::SofaScene::Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)

    Q_PROPERTY(QUrlList canvas READ readCanvas NOTIFY notifyCanvasChanged)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl sourceQML READ sourceQML WRITE setSourceQML NOTIFY sourceQMLChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(QString pathQML READ pathQML NOTIFY pathQMLChanged)
    Q_PROPERTY(double dt READ dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(bool animate READ animate WRITE setAnimate NOTIFY animateChanged)
    Q_PROPERTY(bool defaultAnimate READ defaultAnimate WRITE setDefaultAnimate NOTIFY defaultAnimateChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(bool pyQtSynchronous READ pyQtSynchronous WRITE setPyQtForceSynchronous NOTIFY pyQtForceSynchronousChanged)
    Q_PROPERTY(sofaqtquick::bindings::SofaBase* selectedComponent READ selectedComponent WRITE setSelectedComponent NOTIFY selectedComponentChanged)
    Q_PROPERTY(sofa::qtquick::Manipulator* selectedManipulator READ selectedManipulator WRITE setSelectedManipulator NOTIFY selectedManipulatorChanged)
    Q_PROPERTY(QQmlListProperty<sofa::qtquick::Manipulator> manipulators READ manipulators)

    Q_ENUMS(Status)
    enum Status {
        Null,
        Ready,
        Loading,
        Unloading,
        Error
    };

public:
    sofa::qtquick::SofaScene::Status status()	const               {return myStatus;}
    void setStatus(sofa::qtquick::SofaScene::Status newStatus);

    bool isLoading() const                                      {return Status::Loading == myStatus;}
    bool isReady() const                                        {return Status::Ready == myStatus;}
    bool isOnError() const                                      {return Status::Error == myStatus;}

    const QString& header() const                               {return myHeader;}
    void setHeader(const QString& newHeader);

    /// get the list of url pointing to the canvas currently associated with the
    /// current scene.
    QUrlList readCanvas();
    void addCanvas(const QUrl& canvas);

    /// set the url of the scene, when changed the SofaScene object will goes through
    /// the following states: Unloaded, Null, Loading, Ready.
    /// On error the scene status is set to Error and a new source has to be set.
    void setSource(const QUrl& newSource);
    const QUrl& source() const                                  {return mySource;}

    /// Sets the rootNode of the scene, constructed from C++
    void setCppSceneGraph(SofaComponent* newSource);

    const QUrl& sourceQML() const                               {return mySourceQML;}
    void setSourceQML(const QUrl& newSourceQML);

    const QString& path() const                                 {return myPath;}
    void setPath(const QString& newPath);

    const QString& pathQML() const                              {return myPathQML;}
    void setPathQML(const QString& newPathQML);

    double dt() const                                           {return myDt;}
    void setDt(double newDt);

    bool animate() const                                        {return myAnimate;}
    void setAnimate(bool newAnimate);

    bool defaultAnimate() const                                 {return myDefaultAnimate;}
    void setDefaultAnimate(bool newDefaultAnimate);

    bool asynchronous() const                                   {return myAsynchronous;}
    void setAsynchronous(bool newAsynchronous);

    bool pyQtSynchronous() const                                {return myPyQtForceSynchronous;}
    void setPyQtForceSynchronous(bool newPyQtSynchronous);

    sofaqtquick::bindings::SofaBase* selectedComponent() const     {return mySelectedComponent;}
    void setSelectedComponent(sofaqtquick::bindings::SofaBase* newSelectedComponent);

    sofa::qtquick::Manipulator* selectedManipulator() const     {return mySelectedManipulator;}
    void setSelectedManipulator(sofa::qtquick::Manipulator* newSelectedManipulator);

    QQmlListProperty<sofa::qtquick::Manipulator> manipulators();

signals:
    void notifyCanvasChanged();

    void loaded();                                      /// scene has been loaded and is ready
    void aboutToUnload();                               /// scene is being to be unloaded
    void statusChanged(Status newStatus);
    void rootNodeChanged();
    void headerChanged(const QString& newHeader);
    void sourceChanged(const QUrl& newSource);
    void cppGraphChanged(SofaComponent* newCppGraph);
    void sourceQMLChanged(const QUrl& newSourceQML);
    void pathChanged(const QString& newPath);
    void pathQMLChanged(const QString& newPathQML);
    void dtChanged(double newDt);
    void animateChanged(bool newAnimate);
    void defaultAnimateChanged(bool newDefaultAnimate);
    void asynchronousChanged(bool newAsynchronous);
    void pyQtForceSynchronousChanged(bool newPyQtSynchronous);
    void selectedComponentChanged(sofaqtquick::bindings::SofaBase* newSelectedComponent);
    void selectedManipulatorChanged(sofa::qtquick::Manipulator* newSelectedManipulator);

public:
    /// Returns an object, a node or a data according to its scene path.
    ///
    /// Examples:
    ///    get("/root")
    ///    get("/root/myObject")
    ///    get("/root/myObject.data")
    Q_INVOKABLE sofaqtquick::bindings::SofaBase* get(const QString& path);

    Q_INVOKABLE double radius() const;
    Q_INVOKABLE void computeBoundingBox(QVector3D& min, QVector3D& max) const;
    Q_INVOKABLE void computeBoundingBox(QVector3D& min, QVector3D& max, const QList<SofaComponent*>& roots) const;
    Q_INVOKABLE QString dumpGraph() const;
    Q_INVOKABLE bool reinitComponent(const QString& path);
    Q_INVOKABLE bool removeComponent(SofaBase* sofaComponent);

    Q_INVOKABLE sofa::qtquick::SofaComponent* addNodeTo(sofa::qtquick::SofaComponent* sofaComponent);
    Q_INVOKABLE void addExistingNodeTo(sofa::qtquick::SofaComponent* sofaComponent, sofa::qtquick::SofaComponent* sofaNode);

    Q_INVOKABLE bool createAndAddComponentTo(SofaComponent* sofaComponent, QString name);

    Q_INVOKABLE bool areSameComponent(SofaComponent* sofaComponentA, SofaComponent* sofaComponentB);
    Q_INVOKABLE bool areInSameBranch(SofaComponent* sofaComponentA, SofaComponent* sofaComponentB);
    Q_INVOKABLE void sendGUIEvent(const QString& controlID, const QString& valueName, const QString& value);

public:
    static QVariant linkValue(const sofa::core::objectmodel::BaseLink* link);

    [[deprecated("Replaced by sofaqtuick::helper::setValueFromQVariant")]]
    static QVariant dataValue(const sofa::core::objectmodel::BaseData* data);

    [[deprecated("Replaced by sofaqtuick::helper::createQVariantFromData")]]
    static bool setDataValue(sofa::core::objectmodel::BaseData* data, const QVariant& value);
    static bool setDataLink(sofa::core::objectmodel::BaseData* data, const QString& link);

    [[deprecated("Replaced by sofaqtuick::helper::getDataObjectProperties")]]
    QVariantMap dataObject(const sofa::core::objectmodel::BaseData* data);

    QVariant dataValue(const QString& path) const;
    QVariant dataValue(const SofaComponent* sofaComponent, const QString& name) const;

    void setDataValue(const QString& path, const QVariant& value);
    void setDataValue(SofaComponent* sofaComponent, const QString& name, const QVariant& value);

    Q_INVOKABLE sofaqtquick::bindings::SofaData* data(const QString& path);
    Q_INVOKABLE SofaLink* link(const QString& path);
    Q_INVOKABLE sofa::qtquick::SofaComponent* node(const QString& path);
    Q_INVOKABLE sofa::qtquick::SofaComponent* component(const QString& path);
    Q_INVOKABLE sofa::qtquick::SofaComponent* componentByType(const QString& typeName);
    Q_INVOKABLE sofa::qtquick::SofaComponentList* componentsByType(const QString& typeName);
    Q_INVOKABLE sofaqtquick::bindings::SofaNode* root();

    // TODO: avoid this kind of specialization if possible
    Q_INVOKABLE sofa::qtquick::SofaComponent* visualStyleComponent();

    Q_INVOKABLE bool save(const QString& projectRootDir);
    Q_INVOKABLE bool save2();

    ///Q_INVOKABLE sofa::qtquick::SofaComponent* retrievePythonScriptController(SofaComponent* context, const QString& derivedFrom, const QString& module = "");

protected:
    Q_INVOKABLE QVariant onDataValueByPath(const QString& path) const; /// \note From QML: directly call 'dataValue'
    Q_INVOKABLE QVariant onDataValueByComponent(sofa::qtquick::SofaComponent* sofaComponent, const QString& name) const; /// \note From QML: directly call 'dataValue'

    Q_INVOKABLE void onSetDataValueByPath(const QString& path, const QVariant& value); /// \note From QML: directly call 'setDataValue' with variadic set parameters
    Q_INVOKABLE void onSetDataValueByComponent(sofa::qtquick::SofaComponent* sofaComponent, const QString& name, const QVariant& value); /// \note From QML: directly call 'setDataValue' with variadic set parameters

public slots:
    void reload();
    void step();
    void markVisualDirty(); // useful when you move objects without stepping (with manipulators for instance)
    void reset();
    //void exportObj();

    void onKeyPressed(char key);
    void onKeyReleased(char key);

    void onMouseMove(double x, double y) ;
    void onMousePressed(int button, double x, double y) ;
    void onMouseReleased(int button, double x, double y) ;

signals:
    void stepBegin();
    void stepEnd();
    void reseted();

private slots:
    void open();
    void loadCppGraph();
    void handleStatusChange(Status newStatus);
    void unloadAllCanvas();

public:
    sofa::simulation::Simulation* sofaSimulation() const {return mySofaSimulation;}
    const sofa::simulation::Node::SPtr& sofaRootNode() const {return mySofaRootNode;}
    sofa::simulation::Node::SPtr& sofaRootNode() {return mySofaRootNode;}

    /// Call that before rendering any sofa scene. This insure that the texture & meshes are updated.
    void prepareSceneForDrawing() ;

    sofa::simulation::scenechecking::SceneCheckerVisitor* m_scenechecker;
protected:
    /// \brief      Low-level function for mechanical state particle picking
    /// \note       The best way to pick a particle is to use a Viewer instead of directly call this function
    /// \return     A 'SelectableSceneParticle' containing the picked particle and the SofaComponent where it belongs
    SelectableSofaParticle* pickParticle(const QVector3D& origin, const QVector3D& direction, double distanceToRay, double distanceToRayGrowth, const QStringList& tags, const QList<SofaComponent*>& roots = QList<SofaComponent*>());

private:
    Status                                      myStatus;
    QString                                     myHeader;
    QUrl                                        mySource;
    QUrl                                        mySourceQML;
    QString                                     myPath;
    QString                                     myPathQML;
    mutable bool                                myVisualDirty {true} ;
    mutable bool                                myTextureAreDirty {true} ;
    double                                      myDt;
    bool                                        myAnimate;
    bool                                        myDefaultAnimate;
    bool                                        myAsynchronous;
    bool                                        myPyQtForceSynchronous;

    sofa::simulation::Simulation*               mySofaSimulation;
    sofa::simulation::Node::SPtr                mySofaRootNode;
    QTimer*                                     myStepTimer;

    QList<Manipulator*>                         myManipulators;
    Manipulator*                                mySelectedManipulator;
    SofaBase*                                   mySelectedComponent;

    SofaComponent*                              myCppGraph;

    QUrlList                                    m_canvas;
};

}

}

