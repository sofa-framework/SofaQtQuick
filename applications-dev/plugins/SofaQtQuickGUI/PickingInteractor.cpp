#include "PickingInteractor.h"
#include "Scene.h"
#include "Viewer.h"
#include "Manipulator.h"

#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/CleanupVisitor.h>
#include <sofa/simulation/common/DeleteVisitor.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <SofaBoundaryCondition/FixedConstraint.h>
#include <SofaDeformable/StiffSpringForceField.h>
#include <SofaOpenglVisual/OglModel.h>

#include <qqml.h>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

typedef sofa::simulation::Node Node;
typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3dTypes> MechanicalObject3d;
typedef sofa::component::projectiveconstraintset::FixedConstraint<sofa::defaulttype::Vec3dTypes> FixedConstraint3d;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::Vec3dTypes> StiffSpringForceField3d;

PickingInteractor::PickingInteractor(QObject *parent) : QObject(parent), QQmlParserStatus(),
	myScene(0),
	myNode(0),
	myMechanicalState(0),
	myForcefield(0),
	myDistanceToRay(1.0),
	myDistanceToRayGrowth(0.001),
	myPickedPoint(0),
	myStiffness(100)
{

}

PickingInteractor::~PickingInteractor()
{
	release();
}

void PickingInteractor::classBegin()
{

}

void PickingInteractor::componentComplete()
{
	if(!myScene)
		setScene(qobject_cast<Scene*>(parent()));
}

void PickingInteractor::setScene(Scene* newScene)
{
	if(newScene == myScene)
		return;

	myScene = newScene;

	sceneChanged(newScene);
}

bool PickingInteractor::pickUsingGeometry(const QVector3D& origin, const QVector3D& ray)
{
	release();

	if(!myScene || !myScene->isReady())
		return false;

	sofa::defaulttype::Vector3 direction(ray.x(), ray.y(), ray.z());
	direction.normalize();

    myDistanceToRay = myScene->radius() / 76.0;
    myDistanceToRayGrowth = 0.001;
	sofa::simulation::MechanicalPickParticlesVisitor pickVisitor(sofa::core::ExecParams::defaultInstance(),
																 sofa::defaulttype::Vector3(origin.x(), origin.y(), origin.z()),
																 direction,
																 myDistanceToRay,
																 myDistanceToRayGrowth);

	pickVisitor.execute(myScene->sofaSimulation()->GetRoot()->getContext());

	if(!pickVisitor.particles.empty())
	{
		MechanicalObject3d* pickedPointMechanicalObject = dynamic_cast<MechanicalObject3d*>(pickVisitor.particles.begin()->second.first);
		if(!pickedPointMechanicalObject)
			return false;

		myPickedPoint = new PickedPoint();

		myPickedPoint->mechanicalState = pickedPointMechanicalObject;
		myPickedPoint->index = pickVisitor.particles.begin()->second.second;
        myPickedPoint->model = nullptr;
        myPickedPoint->manipulator = nullptr;
		myPickedPoint->position = QVector3D(myPickedPoint->mechanicalState->getPX(myPickedPoint->index),
											myPickedPoint->mechanicalState->getPY(myPickedPoint->index),
											myPickedPoint->mechanicalState->getPZ(myPickedPoint->index));

		MechanicalObject3d::SPtr mechanicalObject = sofa::core::objectmodel::New<MechanicalObject3d>();
		mechanicalObject->setName("Attractor");
		mechanicalObject->resize(1);
		mechanicalObject->writePositions()[0] = sofa::defaulttype::Vector3(myPickedPoint->position.x(), myPickedPoint->position.y(), myPickedPoint->position.z());
		myMechanicalState = mechanicalObject.get();

		FixedConstraint3d::SPtr fixedConstraint = sofa::core::objectmodel::New<FixedConstraint3d>();

		StiffSpringForceField3d::SPtr stiffSpringForcefield = sofa::core::objectmodel::New<StiffSpringForceField3d>(mechanicalObject.get(), pickedPointMechanicalObject);
		stiffSpringForcefield->setName("Spring");
		stiffSpringForcefield->addSpring(0, myPickedPoint->index, myStiffness, 0.1, 0.0);
		myForcefield = stiffSpringForcefield.get();

		Node::SPtr node = myScene->sofaSimulation()->GetRoot()->createChild("Interactor");
        node->addObject(mechanicalObject);
        node->addObject(fixedConstraint);
        node->addObject(stiffSpringForcefield);
		node->init(sofa::core::ExecParams::defaultInstance());
		myNode = node.get();

        Node* pickedNode = dynamic_cast<Node*>(stiffSpringForcefield->getMState2()->getContext());
        pickedNode->moveObject(stiffSpringForcefield);

		pickingChanged(true);

		return true;
	}

	return false;
}

bool PickingInteractor::pickUsingRasterization(Viewer* viewer, const QPointF& ssPoint)
{
    release();

    if(!myScene || !myScene->isReady())
        return false;

    SceneComponent* sceneComponent = nullptr;
    Manipulator* manipulator = nullptr;
    QVector3D position;

    if(viewer->pickUsingRasterization(ssPoint, sceneComponent, manipulator, position))
    {
        OglModel* model = nullptr;
        if(sceneComponent)
        {
            model = dynamic_cast<OglModel*>(sceneComponent->base());
            if(!model)
                return false;
        }

        myPickedPoint = new PickedPoint();

        myPickedPoint->mechanicalState = nullptr;
        myPickedPoint->index = -1;
        myPickedPoint->model = model;
        myPickedPoint->manipulator = manipulator;
        myPickedPoint->position = position;

        return true;
    }

    return false;
}

QVector3D PickingInteractor::pickedPosition() const
{
	if(!myPickedPoint)
		return QVector3D();

	return myPickedPoint->position;
}

sofa::qtquick::Manipulator* PickingInteractor::pickedManipulator() const
{
    if(!myPickedPoint)
        return nullptr;

    return myPickedPoint->manipulator;
}

sofa::qtquick::SceneComponent* PickingInteractor::pickedMechanicalObject() const
{
    if(!myPickedPoint || !myPickedPoint->mechanicalState)
        return nullptr;

    return new SceneComponent(myScene, myPickedPoint->mechanicalState);
}

sofa::qtquick::SceneComponent* PickingInteractor::pickedOglModel() const
{
    if(!myPickedPoint || !myPickedPoint->model)
        return nullptr;

    return new SceneComponent(myScene, myPickedPoint->model);
}

QVector3D PickingInteractor::position() const
{
	if(!myMechanicalState)
		return QVector3D();

	MechanicalObject3d* mechanicalObject = static_cast<MechanicalObject3d*>(myMechanicalState);
	sofa::defaulttype::Vector3 position = mechanicalObject->readPositions()[0];

	return QVector3D(position.x(), position.y(), position.z());
}

void PickingInteractor::setPosition(const QVector3D& position)
{
    if(!myPickedPoint)
		return;

    if(myMechanicalState)
    {
        MechanicalObject3d* mechanicalObject = static_cast<MechanicalObject3d*>(myMechanicalState);
        mechanicalObject->writePositions()[0] = sofa::defaulttype::Vector3(position.x(), position.y(), position.z());
        positionChanged(position);
    }
}

void PickingInteractor::release()
{
	bool picking = PickingInteractor::picking();

	if(myNode)
	{
        StiffSpringForceField3d::SPtr stiffSpringForcefield = static_cast<StiffSpringForceField3d*>(myForcefield);
        myNode->moveObject(stiffSpringForcefield);

		Node::SPtr node = static_cast<Node*>(myNode);
		node->detachFromGraph();
		node->execute<sofa::simulation::CleanupVisitor>(sofa::core::ExecParams::defaultInstance());
		node->execute<sofa::simulation::DeleteVisitor>(sofa::core::ExecParams::defaultInstance());
	}

	delete myPickedPoint;
	myPickedPoint = 0;

	myNode = 0;
	myMechanicalState = 0;
	myForcefield = 0;

	if(picking)
		pickingChanged(false);
}

}

}
