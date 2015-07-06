#ifndef PARTICLEINTERACTOR_H
#define PARTICLEINTERACTOR_H

#include "SofaQtQuickGUI.h"
#include "Viewer.h"
#include "SceneComponent.h"

#include <QObject>
#include <QVector3D>

namespace sofa
{

namespace core
{

namespace behavior
{
	class BaseMechanicalState;
	class BaseInteractionForceField;
}

namespace objectmodel
{
	class BaseNode;
}

}

namespace component
{

namespace visualmodel
{
    class OglModel;
}

}

namespace qtquick
{

class SceneComponent;
class Scene;
class Manipulator;

class SOFA_SOFAQTQUICKGUI_API ParticleInteractor : public QObject
{
    Q_OBJECT

	typedef sofa::core::behavior::BaseMechanicalState			BaseMechanicalState;
	typedef sofa::core::behavior::BaseInteractionForceField		BaseInteractionForceField;
	typedef sofa::core::objectmodel::BaseNode					BaseNode;
    typedef sofa::component::visualmodel::OglModel              OglModel;

public:
    ParticleInteractor(QObject *parent = 0);
    ~ParticleInteractor();
	
public:
    Q_PROPERTY(sofa::qtquick::SceneComponent* sceneComponent MEMBER mySceneComponent NOTIFY sceneComponentChanged)
    Q_PROPERTY(int particleIndex MEMBER myParticleIndex NOTIFY particleIndexChanged)
    Q_PROPERTY(double stiffness MEMBER myStiffness NOTIFY stiffnessChanged)
    Q_PROPERTY(QVector3D interactorPosition READ interactorPosition NOTIFY interactorPositionChanged)
    Q_PROPERTY(bool interacting READ interacting NOTIFY interactingChanged)

public:
    SceneComponent* sceneComponent() const      {return mySceneComponent;}
    double stiffness() const                    {return myStiffness;}
    QVector3D interactorPosition() const;
    bool interacting() const;

signals:
    void sceneComponentChanged(sofa::qtquick::SceneComponent* newSceneComponent);
    void particleIndexChanged(double newParticleIndex);
	void stiffnessChanged(double newStiffness);
    void interactorPositionChanged(const QVector3D& newInteractorPosition);
    void interactingChanged(bool newInteracting);

public:
    Q_INVOKABLE QVector3D particlePosition() const;
	
public slots:
    bool start(SceneComponent* sceneComponent, int particleIndex);
    bool update(const QVector3D& interactorNewPosition);
    void release();

private:
    SceneComponent*                         mySceneComponent;
    int                                     myParticleIndex;
    double									myStiffness;

	BaseNode*								myNode;
	BaseMechanicalState*					myMechanicalState;
    BaseInteractionForceField*				myForcefield;
};

}

}

#endif // PARTICLEINTERACTOR_H
