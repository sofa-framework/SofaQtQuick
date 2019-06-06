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

#ifndef SOFASCENE_H
#define SOFASCENE_H

#include "SofaQtQuickGUI.h"
#include "SofaComponent.h"
#include "SofaComponentList.h"
#include "SofaData.h"
#include "Manipulator.h"
#include "SelectableSofaParticle.h"

#include <sofa/simulation/Simulation.h>
#include <sofa/simulation/MutationListener.h>

#include <QObject>
#include <QQmlListProperty>
#include <QVariant>
#include <QSet>
#include <QVector3D>
#include <QUrl>

class QTimer;
class QOpenGLShaderProgram;
class QOpenGLFramebufferObject;
class QOffscreenSurface;

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

/// \class QtQuick wrapper for a Sofa scene, allowing us to simulate, modify and draw (basic function) a Sofa scene
class SOFA_SOFAQTQUICKGUI_API SofaScene : public QObject, private sofa::simulation::MutationListener
{
    Q_OBJECT

    friend class SofaViewer;
    friend class PickUsingRasterizationWorker;
    friend bool LoaderProcess(SofaScene* scene, QOffscreenSurface* surface);

public:
    explicit SofaScene(QObject *parent = 0);
    ~SofaScene();

public:
    Q_PROPERTY(sofa::qtquick::SofaScene::Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl sourceQML READ sourceQML WRITE setSourceQML NOTIFY sourceQMLChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(QString pathQML READ pathQML NOTIFY pathQMLChanged)
    Q_PROPERTY(double dt READ dt WRITE setDt NOTIFY dtChanged)
    Q_PROPERTY(bool animate READ animate WRITE setAnimate NOTIFY animateChanged)
    Q_PROPERTY(bool defaultAnimate READ defaultAnimate WRITE setDefaultAnimate NOTIFY defaultAnimateChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(bool pyQtSynchronous READ pyQtSynchronous WRITE setPyQtForceSynchronous NOTIFY pyQtForceSynchronousChanged)
    Q_PROPERTY(sofa::qtquick::SofaComponent* selectedComponent READ selectedComponent WRITE setSelectedComponent NOTIFY selectedComponentChanged)
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

    const QString& header() const                               {return myHeader;}
    void setHeader(const QString& newHeader);

    const QUrl& source() const                                  {return mySource;}
    void setSource(const QUrl& newSource);

    const QUrl& sourceQML() const                               {return mySourceQML;}
    void setSourceQML(const QUrl& newSourceQML);

    const QString& path() const                                 {return myPath;}
    void setPath(const QString& newPath);

    const QString& pathQML() const                              {return myPathQML;}
    void setPathQML(const QString& newPathQML);

    const std::vector<std::string>& extraParams() const         {return myExtraParams;}
    void setExtraParams(const std::vector<std::string>& newExtraParams);

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

    sofa::qtquick::SofaComponent* selectedComponent() const     {return mySelectedComponent;}
    void setSelectedComponent(sofa::qtquick::SofaComponent* newSelectedComponent);

    sofa::qtquick::Manipulator* selectedManipulator() const     {return mySelectedManipulator;}
    void setSelectedManipulator(sofa::qtquick::Manipulator* newSelectedManipulator);

    QQmlListProperty<sofa::qtquick::Manipulator> manipulators();

signals:
    void loaded();                                      /// scene has been loaded and is ready
    void aboutToUnload();                               /// scene is being to be unloaded
    void statusChanged(Status newStatus);
    void headerChanged(const QString& newHeader);
    void sourceChanged(const QUrl& newSource);
    void sourceQMLChanged(const QUrl& newSourceQML);
    void pathChanged(const QString& newPath);
    void pathQMLChanged(const QString& newPathQML);
    void dtChanged(double newDt);
    void animateChanged(bool newAnimate);
    void defaultAnimateChanged(bool newDefaultAnimate);
    void asynchronousChanged(bool newAsynchronous);
    void pyQtForceSynchronousChanged(bool newPyQtSynchronous);
    void selectedComponentChanged(sofa::qtquick::SofaComponent* newSelectedComponent);
    void selectedManipulatorChanged(sofa::qtquick::Manipulator* newSelectedManipulator);

public:
    Q_INVOKABLE double radius() const;
    Q_INVOKABLE void computeBoundingBox(QVector3D& min, QVector3D& max) const;
    Q_INVOKABLE void computeBoundingBox(QVector3D& min, QVector3D& max, const QList<SofaComponent*>& roots) const;
    Q_INVOKABLE QString dumpGraph() const;
    Q_INVOKABLE bool reinitComponent(const QString& path);
    Q_INVOKABLE bool removeComponent(SofaComponent* sofaComponent);
    Q_INVOKABLE bool addNodeTo(SofaComponent* sofaComponent);

    Q_INVOKABLE bool areSameComponent(SofaComponent* sofaComponentA, SofaComponent* sofaComponentB);
    Q_INVOKABLE bool areInSameBranch(SofaComponent* sofaComponentA, SofaComponent* sofaComponentB);
    Q_INVOKABLE void sendGUIEvent(const QString& controlID, const QString& valueName, const QString& value);

public:
    static QVariant linkValue(const sofa::core::objectmodel::BaseLink* link);
    static QVariant dataValue(const sofa::core::objectmodel::BaseData* data);
    static bool setDataValue(sofa::core::objectmodel::BaseData* data, const QVariant& value);
    static bool setDataLink(sofa::core::objectmodel::BaseData* data, const QString& link);

    QVariantMap dataObject(const sofa::core::objectmodel::BaseData* data);

    QVariant dataValue(const QString& path) const;
    QVariant dataValue(const SofaComponent* sofaComponent, const QString& name) const;

    void setDataValue(const QString& path, const QVariant& value);
    void setDataValue(SofaComponent* sofaComponent, const QString& name, const QVariant& value);

    Q_INVOKABLE sofa::qtquick::SofaData* data(const QString& path);
    Q_INVOKABLE sofa::qtquick::SofaComponent* component(const QString& path);
    Q_INVOKABLE sofa::qtquick::SofaComponent* componentByType(const QString& typeName);
    Q_INVOKABLE sofa::qtquick::SofaComponentList* componentsByType(const QString& typeName);
    Q_INVOKABLE sofa::qtquick::SofaComponent* root();

    bool componentExists(const sofa::core::objectmodel::Base* base) const;

    // TODO: avoid this kind of specialization if possible
    Q_INVOKABLE sofa::qtquick::SofaComponent* visualStyleComponent();
    Q_INVOKABLE sofa::qtquick::SofaComponent* retrievePythonScriptController(SofaComponent* context, const QString& derivedFrom, const QString& module = "");

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

signals:
    void stepBegin();
    void stepEnd();
    void reseted();

private slots:
    void open();
    void handleStatusChange(Status newStatus);

public:
    sofa::simulation::Simulation* sofaSimulation() const {return mySofaSimulation;}
    const sofa::simulation::Node::SPtr& sofaRootNode() const {return mySofaRootNode;}
    sofa::simulation::Node::SPtr& sofaRootNode() {return mySofaRootNode;}

protected:
    /// \brief      Low-level drawing function
    /// \attention  Require an opengl context bound to a surface, viewport / projection / modelview must have been set
    /// \note       The best way to display a 'Scene' is to use a 'Viewer' instead of directly call this function
    void draw(const SofaViewer& viewer, const QList<SofaComponent*>& roots = QList<SofaComponent*>()) const;

    /// \brief      Low-level function for mechanical state particle picking
    /// \note       The best way to pick a particle is to use a Viewer instead of directly call this function
    /// \return     A 'SelectableSceneParticle' containing the picked particle and the SofaComponent where it belongs
    SelectableSofaParticle* pickParticle(const QVector3D& origin, const QVector3D& direction, double distanceToRay, double distanceToRayGrowth, const QStringList& tags, const QList<SofaComponent*>& roots = QList<SofaComponent*>());

    /// \brief      Low-level function for color index picking
    /// \attention  Require an opengl context bound to a surface, viewport / projection / modelview must have been set
    /// \note       The best way to pick an object is to use a Viewer instead of directly call this function
    /// \return     A 'Selectable' containing the picked object
    Selectable* pickObject(const SofaViewer& viewer, const QPointF& ssPoint, const QStringList& tagList, const QList<SofaComponent*>& roots = QList<SofaComponent*>());

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
    QString                                     myPath;
    QString                                     myPathQML;
    std::vector<std::string>					myExtraParams;
    mutable bool                                myVisualDirty;
    double                                      myDt;
    bool                                        myAnimate;
    bool                                        myDefaultAnimate;
    bool                                        myAsynchronous;
    bool                                        myPyQtForceSynchronous;

    sofa::simulation::Simulation*               mySofaSimulation;
    sofa::simulation::Node::SPtr                mySofaRootNode;
    QTimer*                                     myStepTimer;
    QSet<const sofa::core::objectmodel::Base*>  myBases;                        /// \todo For each base, reference a unique SofaComponent and use it in QML as a wrapper

    QList<Manipulator*>                         myManipulators;
    Manipulator*                                mySelectedManipulator;
    SofaComponent*                              mySelectedComponent;

    QOpenGLShaderProgram*                       myHighlightShaderProgram;
    QOpenGLShaderProgram*                       myPickingShaderProgram;
    QOpenGLFramebufferObject*                   myPickingFBO;
};

}

}

#endif // SOFASCENE_H
