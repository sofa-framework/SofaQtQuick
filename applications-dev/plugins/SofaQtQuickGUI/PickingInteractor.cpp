#include "PickingInteractor.h"
#include "Scene.h"

#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/CleanupVisitor.h>
#include <sofa/simulation/common/DeleteVisitor.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <SofaBoundaryCondition/FixedConstraint.h>
#include <SofaDeformable/StiffSpringForceField.h>

#include <qqml.h>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

typedef sofa::simulation::Node Node;
typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3Types> MechanicalObjectVec3;
typedef sofa::component::projectiveconstraintset::FixedConstraint<sofa::defaulttype::Vec3Types> FixedConstraintVec3;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::Vec3Types> StiffSpringForceFieldVec3;

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
	connect(this, &PickingInteractor::sceneChanged, this, &PickingInteractor::handleSceneChanged);
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

void PickingInteractor::handleSceneChanged(Scene* scene)
{
	if(scene)
	{
		if(scene->isReady())
			computePickProperties();

		connect(scene, &Scene::loaded, this, &PickingInteractor::computePickProperties);
	}
}

void PickingInteractor::computePickProperties()
{
	if(!myScene)
		return;

	myDistanceToRay = myScene->radius() / 76.0;
	myDistanceToRayGrowth = 0.001;
}

bool PickingInteractor::pick(const QVector3D& origin, const QVector3D& ray)
{
	release();

	if(!myScene || !myScene->isReady())
		return false;

	sofa::defaulttype::Vector3 direction(ray.x(), ray.y(), ray.z());
	direction.normalize();

	sofa::simulation::MechanicalPickParticlesVisitor pickVisitor(sofa::core::ExecParams::defaultInstance(),
																 sofa::defaulttype::Vector3(origin.x(), origin.y(), origin.z()),
																 direction,
																 myDistanceToRay,
																 myDistanceToRayGrowth);

	pickVisitor.execute(myScene->sofaSimulation()->GetRoot()->getContext());

	if(!pickVisitor.particles.empty())
	{
        MechanicalObjectVec3* pickedPointMechanicalObject = dynamic_cast<MechanicalObjectVec3*>(pickVisitor.particles.begin()->second.first);
		if(!pickedPointMechanicalObject)
			return false;

		myPickedPoint = new PickedPoint();

		myPickedPoint->mechanicalState = pickedPointMechanicalObject;
		myPickedPoint->index = pickVisitor.particles.begin()->second.second;
		myPickedPoint->position = QVector3D(myPickedPoint->mechanicalState->getPX(myPickedPoint->index),
											myPickedPoint->mechanicalState->getPY(myPickedPoint->index),
											myPickedPoint->mechanicalState->getPZ(myPickedPoint->index));

        MechanicalObjectVec3::SPtr mechanicalObject = sofa::core::objectmodel::New<MechanicalObjectVec3>();
		mechanicalObject->setName("Attractor");
		mechanicalObject->resize(1);
		mechanicalObject->writePositions()[0] = sofa::defaulttype::Vector3(myPickedPoint->position.x(), myPickedPoint->position.y(), myPickedPoint->position.z());
		myMechanicalState = mechanicalObject.get();

        FixedConstraintVec3::SPtr fixedConstraint = sofa::core::objectmodel::New<FixedConstraintVec3>();

        StiffSpringForceFieldVec3::SPtr stiffSpringForcefield = sofa::core::objectmodel::New<StiffSpringForceFieldVec3>(mechanicalObject.get(), pickedPointMechanicalObject);
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

QVector3D PickingInteractor::pickedPointPosition() const
{
	if(!myPickedPoint)
		return QVector3D();

	return myPickedPoint->position;
}

QVector3D PickingInteractor::position() const
{
	if(!myMechanicalState)
		return QVector3D();

    MechanicalObjectVec3* mechanicalObject = static_cast<MechanicalObjectVec3*>(myMechanicalState);
	sofa::defaulttype::Vector3 position = mechanicalObject->readPositions()[0];

	return QVector3D(position.x(), position.y(), position.z());
}

void PickingInteractor::setPosition(const QVector3D& position)
{
    if(!myPickedPoint || !myMechanicalState)
		return;

    MechanicalObjectVec3* mechanicalObject = static_cast<MechanicalObjectVec3*>(myMechanicalState);
	mechanicalObject->writePositions()[0] = sofa::defaulttype::Vector3(position.x(), position.y(), position.z());

	positionChanged(position);
}

void PickingInteractor::release()
{
	bool picking = PickingInteractor::picking();

	if(myNode)
	{
        StiffSpringForceFieldVec3::SPtr stiffSpringForcefield = static_cast<StiffSpringForceFieldVec3*>(myForcefield);
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
