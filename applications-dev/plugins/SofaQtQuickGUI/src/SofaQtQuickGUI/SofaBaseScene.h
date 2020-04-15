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
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
#include <SofaQtQuickGUI/Bindings/SofaBaseObject.h>
#include <SofaQtQuickGUI/Bindings/SofaNode.h>

#include <SofaQtQuickGUI/SofaComponentList.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>
#include <SofaQtQuickGUI/Bindings/SofaLink.h>
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

}

namespace sofaqtquick
{

class SofaBaseScene;
class SofaViewer;
class PickUsingRasterizationWorker;
using sofaqtquick::bindings::SofaBase;
using sofaqtquick::bindings::SofaBaseObject;
using sofaqtquick::bindings::SofaBaseObjectList;
using sofaqtquick::bindings::SofaNode;

typedef QList<QUrl> QUrlList;

/// \class QtQuick wrapper for a Sofa scene, allowing us to simulate,
/// modify and draw (basic function) a Sofa scene
class SOFA_SOFAQTQUICKGUI_API SofaBaseScene : public QObject
{
    Q_OBJECT

    friend class SofaViewer;
    friend class CameraView;
    friend class EditView;

    friend class PickUsingRasterizationWorker;
    friend bool LoaderProcess(SofaBaseScene* scene);

public:
    explicit SofaBaseScene(QObject *parent = nullptr);
    ~SofaBaseScene();

public:
    Q_PROPERTY(sofaqtquick::SofaBaseScene::Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)

    Q_PROPERTY(QList<QObject*> canvas READ readCanvas NOTIFY notifyCanvasChanged)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl sourceQML READ sourceQML WRITE setSourceQML NOTIFY sourceQMLChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(QString pathQML READ pathQML NOTIFY pathQMLChanged)
    Q_PROPERTY(double dt READ dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(bool animate READ animate WRITE setAnimate NOTIFY animateChanged)
    Q_PROPERTY(bool defaultAnimate READ defaultAnimate WRITE setDefaultAnimate NOTIFY defaultAnimateChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(bool pyQtSynchronous READ pyQtSynchronous WRITE setPyQtForceSynchronous NOTIFY pyQtForceSynchronousChanged)
    Q_PROPERTY(SofaNode* rootNode READ getRootNode NOTIFY rootNodeChanged)

    Q_ENUMS(Status)
    enum Status {
        Null,
        Ready,
        Loading,
        Unloading,
        Error
    };




public:
    /// Scene-related File Menu methods:
    Q_INVOKABLE void newScene();
    Q_INVOKABLE void openScene(QUrl projectDir);
    Q_INVOKABLE void reloadScene();
    Q_INVOKABLE void saveScene(QString sceneFile = "");
    Q_INVOKABLE void saveSceneAs(QUrl projectDir);
    Q_INVOKABLE void exportSceneAs(QUrl projectDir);



public:
    sofaqtquick::SofaBaseScene::Status status()	const               {return myStatus;}
    void setStatus(sofaqtquick::SofaBaseScene::Status newStatus);

    bool isLoading() const                                      {return Status::Loading == myStatus;}
    bool isReady() const                                        {return Status::Ready == myStatus;}
    bool isOnError() const                                      {return Status::Error == myStatus;}

    const QString& header() const                               {return myHeader;}
    void setHeader(const QString& newHeader);

    /// get the list of url pointing to the canvas currently associated with the
    /// current scene.
    QList<QObject*> readCanvas();
    Q_INVOKABLE void checkForCanvases();
    void addCanvas(const QUrl& canvas);

    /// set the url of the scene, when changed the SofaScene object will goes through
    /// the following states: Unloaded, Null, Loading, Ready.
    /// On error the scene status is set to Error and a new source has to be set.
    void setSource(const QUrl& newSource);
    const QUrl& source() const                                  {return mySource;}

    /// Sets the rootNode of the scene, constructed from C++
    void setCppSceneGraph(SofaBase* newSource);

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


signals:
    void notifyCanvasChanged();

    void loaded();                                      /// scene has been loaded and is ready
    void aboutToUnload();                               /// scene is being to be unloaded
    void statusChanged(Status newStatus);
    void rootNodeChanged();
    void headerChanged(const QString& newHeader);
    void sourceChanged(const QUrl& newSource);
    void cppGraphChanged(SofaBase* newCppGraph);
    void sourceQMLChanged(const QUrl& newSourceQML);
    void pathChanged(const QString& newPath);
    void pathQMLChanged(const QString& newPathQML);
    void dtChanged(double newDt);
    void animateChanged(bool newAnimate);
    void defaultAnimateChanged(bool newDefaultAnimate);
    void asynchronousChanged(bool newAsynchronous);
    void pyQtForceSynchronousChanged(bool newPyQtSynchronous);

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
    Q_INVOKABLE void computeBoundingBox(QVector3D& min, QVector3D& max, const QList<SofaBase*>& roots) const;
    Q_INVOKABLE QString dumpGraph() const;
    Q_INVOKABLE bool reinitComponent(const QString& path);
    Q_INVOKABLE bool removeComponent(SofaBase* SofaBase);

    Q_INVOKABLE sofaqtquick::SofaBase* addNodeTo(sofaqtquick::SofaBase* SofaBase);
    Q_INVOKABLE void addExistingNodeTo(sofaqtquick::SofaBase* SofaBase, sofaqtquick::SofaBase* sofaNode);

    Q_INVOKABLE bool createAndAddComponentTo(SofaBase* SofaBase, QString name);

    Q_INVOKABLE bool areSameComponent(SofaBase* SofaBaseA, SofaBase* SofaBaseB);
    Q_INVOKABLE bool areInSameBranch(SofaBase* SofaBaseA, SofaBase* SofaBaseB);
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
    QVariant dataValue(const SofaBase* SofaBase, const QString& name) const;

    void setDataValue(const QString& path, const QVariant& value);
    void setDataValue(SofaBase* SofaBase, const QString& name, const QVariant& value);

    Q_INVOKABLE sofaqtquick::bindings::SofaData* data(const QString& path);
    Q_INVOKABLE sofaqtquick::bindings::SofaLink* link(const QString& path);
    Q_INVOKABLE sofaqtquick::bindings::SofaBase* node(const QString& path);
    Q_INVOKABLE sofaqtquick::bindings::SofaBase* component(const QString& path);
    Q_INVOKABLE sofaqtquick::bindings::SofaBaseObject* componentByType(const QString& typeName);
    Q_INVOKABLE SofaBaseObjectList* componentsByType(const QString& typeName);
    Q_INVOKABLE sofaqtquick::bindings::SofaNode* root();
    sofaqtquick::bindings::SofaNode* getRootNode() { return root(); }

    // TODO: avoid this kind of specialization if possible
    Q_INVOKABLE sofaqtquick::SofaBase* visualStyleComponent();

    //Q_INVOKABLE bool save(const QString& projectRootDir);
    //Q_INVOKABLE bool save2();

    ///Q_INVOKABLE sofa::qtquick::SofaBase* retrievePythonScriptController(SofaBase* context, const QString& derivedFrom, const QString& module = "");

protected:
    Q_INVOKABLE QVariant onDataValueByPath(const QString& path) const; /// \note From QML: directly call 'dataValue'
    Q_INVOKABLE QVariant onDataValueByComponent(sofaqtquick::SofaBase* SofaBase, const QString& name) const; /// \note From QML: directly call 'dataValue'

    Q_INVOKABLE void onSetDataValueByPath(const QString& path, const QVariant& value); /// \note From QML: directly call 'setDataValue' with variadic set parameters
    Q_INVOKABLE void onSetDataValueByComponent(sofaqtquick::SofaBase* SofaBase, const QString& name, const QVariant& value); /// \note From QML: directly call 'setDataValue' with variadic set parameters

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
    sofa::simulation::Simulation* sofaSimulation() const { return mySofaSimulation; }
    const sofa::simulation::Node::SPtr& sofaRootNode() const { return mySofaRootNode; }
    sofa::simulation::Node::SPtr& sofaRootNode() { return mySofaRootNode; }
    void setSofaRootNode(sofa::simulation::Node::SPtr node) {
        mySofaRootNode = node;
        markVisualDirty();
        myTextureAreDirty = true;
    }

    /// Call that before rendering any sofa scene. This insure that the texture & meshes are updated.
    void prepareSceneForDrawing() ;

    sofa::simulation::scenechecking::SceneCheckerVisitor* m_scenechecker;
protected:
    /// \brief      Low-level function for mechanical state particle picking
    /// \note       The best way to pick a particle is to use a Viewer instead of directly call this function
    /// \return     A 'SelectableSceneParticle' containing the picked particle and the SofaBase where it belongs
    SelectableSofaParticle* pickParticle(const QVector3D& origin, const QVector3D& direction, double distanceToRay, double distanceToRayGrowth, const QStringList& tags, const QList<SofaBase*>& roots = QList<SofaBase*>());

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

    SofaBase*                                   mySelectedComponent {nullptr};

    SofaBase*                                   myCppGraph;

    QList<QObject*>                             m_canvas;
};

}  // namespace sofaqtquick

