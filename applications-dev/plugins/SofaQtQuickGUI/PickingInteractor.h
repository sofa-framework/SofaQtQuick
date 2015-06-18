#ifndef PICKINGINTERACTOR_H
#define PICKINGINTERACTOR_H

#include "SofaQtQuickGUI.h"
#include "Viewer.h"

#include <QObject>
#include <QQmlParserStatus>
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

class SOFA_SOFAQTQUICKGUI_API PickingInteractor : public QObject, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)

	typedef sofa::core::behavior::BaseMechanicalState			BaseMechanicalState;
	typedef sofa::core::behavior::BaseInteractionForceField		BaseInteractionForceField;
	typedef sofa::core::objectmodel::BaseNode					BaseNode;
    typedef sofa::component::visualmodel::OglModel              OglModel;

	struct PickedPoint {
		BaseMechanicalState*	mechanicalState;
		int						index;
        OglModel*               model;
        Manipulator*            manipulator;
		QVector3D				position;
	};

public:
	PickingInteractor(QObject *parent = 0);
	~PickingInteractor();

	void classBegin();
	void componentComplete();
	
public:
    Q_PROPERTY(sofa::qtquick::Scene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(double stiffness MEMBER myStiffness NOTIFY stiffnessChanged)
    Q_PROPERTY(bool picking READ picking NOTIFY pickingChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)

public:
	Scene* scene() const		{return myScene;}
	void setScene(Scene* newScene);

	double stiffness() const	{return myStiffness;}

	bool picking() const		{return 0 != myPickedPoint;}

	QVector3D position() const;
	void setPosition(const QVector3D& position);
	
signals:
    void sceneChanged(sofa::qtquick::Scene* newScene);
	void stiffnessChanged(double newStiffness);
	void pickingChanged(bool newPicking);
	void positionChanged(const QVector3D& newPosition);
	
public slots:
	void release();

public:
    Q_INVOKABLE bool pickUsingGeometry(const QVector3D& origin, const QVector3D& ray);
    Q_INVOKABLE bool pickUsingRasterization(sofa::qtquick::Viewer* viewer, const QPointF& ssPoint);

    Q_INVOKABLE QVector3D                      pickedPosition() const;
    Q_INVOKABLE sofa::qtquick::Manipulator*    pickedManipulator() const;
    Q_INVOKABLE sofa::qtquick::SceneComponent* pickedMechanicalObject() const;
    Q_INVOKABLE sofa::qtquick::SceneComponent* pickedOglModel() const;

private:
	Scene*									myScene;

	BaseNode*								myNode;
	BaseMechanicalState*					myMechanicalState;
	BaseInteractionForceField*				myForcefield;

	double									myDistanceToRay;
	double									myDistanceToRayGrowth;

	PickedPoint*							myPickedPoint;
	double									myStiffness;
	
};

}

}

#endif // PICKINGINTERACTOR_H
