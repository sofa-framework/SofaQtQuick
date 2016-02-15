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

#include "SofaParticleInteractor.inl"
#include "SofaScene.h"
#include "SofaViewer.h"
#include "Manipulator.h"

#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/VisualModel.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/CleanupVisitor.h>
#include <sofa/simulation/common/DeleteVisitor.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <SofaBoundaryCondition/FixedConstraint.h>
#include <SofaRigid/JointSpringForceField.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <sofa/helper/cast.h>

#include <qqml.h>
#include <QDebug>

namespace sofa
{

typedef sofa::simulation::Node Node;

namespace qtquick
{

using namespace sofa::defaulttype;
using namespace sofa::core::behavior;
using namespace sofa::component::container;
using namespace sofa::component::projectiveconstraintset;
using namespace sofa::component::interactionforcefield;

SofaParticleInteractor::SofaParticleInteractor(QObject *parent) : QObject(parent),
    myStiffness(100),
	myParticleInteraction(nullptr)
{
	
}

SofaParticleInteractor::~SofaParticleInteractor()
{
	release();
}

QVector3D SofaParticleInteractor::interactorPosition() const
{
	if(!interacting())
        return QVector3D(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());

	return myParticleInteraction->position();
}

bool SofaParticleInteractor::interacting() const
{
    return myParticleInteraction;
}

bool SofaParticleInteractor::start(SofaComponent* sofaComponent, int particleIndex)
{
    release();

	if(!sofaComponent || -1 == particleIndex)
        return false;

    const SofaScene* sofaScene = sofaComponent->sofaScene();

	BaseMechanicalState* particleMechanicalState = dynamic_cast<BaseMechanicalState*>(sofaComponent->base());

	if(!sofaScene || !particleMechanicalState)
		return false;

	MechanicalObject<Vec3Types>* mechanicalObject_Vec3Types = dynamic_cast<MechanicalObject<Vec3Types>*>(particleMechanicalState);
	if(mechanicalObject_Vec3Types)
	{
		myParticleInteraction = new SofaParticleInteraction<Vec3Types>(sofaComponent, particleIndex, myStiffness);
	}
	else
	{
		MechanicalObject<Rigid3Types>* mechanicalObject_Rigid3Types = dynamic_cast<MechanicalObject<Rigid3Types>*>(particleMechanicalState);
		if(mechanicalObject_Rigid3Types)
			myParticleInteraction = new SofaParticleInteraction<Rigid3Types>(sofaComponent, particleIndex, myStiffness);
	}

	if(!myParticleInteraction)
		return false;

	myParticleInteraction->start();

    interactorPositionChanged(myParticleInteraction->position());
    interactingChanged(true);

    return true;
}

bool SofaParticleInteractor::update(const QVector3D& position)
{
    if(!interacting())
        return false;

    myParticleInteraction->update(position);
	
	interactorPositionChanged(position);

    return true;
}

void SofaParticleInteractor::release()
{
	if(!interacting())
		return;

    myParticleInteraction->release();

	delete myParticleInteraction;
	myParticleInteraction = 0;

    interactingChanged(false);
}

SofaComponent* SofaParticleInteractor::sofaComponent() const
{
	if(!interacting())
		return nullptr;

	return myParticleInteraction->sofaComponent();
}

double SofaParticleInteractor::stiffness() const
{
	return myStiffness;
}

void SofaParticleInteractor::setStiffness(double stiffness)
{
	if(stiffness == myStiffness)
		return;

	myStiffness = stiffness;

	stiffnessChanged(stiffness);
}

// specialization

template<>
inline BaseInteractionForceField::SPtr SofaParticleInteraction<Rigid3fTypes>::createInteractionForceField() const
{
	typedef sofa::component::interactionforcefield::JointSpringForceField<Rigid3fTypes> JointSpringForceFieldRigid3fTypes;
	JointSpringForceFieldRigid3fTypes::SPtr interactionForceField = sofa::core::objectmodel::New<JointSpringForceFieldRigid3fTypes>(myInteractorMechanicalState, myInteractedMechanicalObject);
	sofa::component::interactionforcefield::JointSpring<Rigid3fTypes> spring(0, myParticleIndex);
	spring.setSoftStiffnessTranslation(myStiffness);

	interactionForceField->setName("Spring");
	interactionForceField->addSpring(spring);

	return interactionForceField;
}

template<>
inline BaseInteractionForceField::SPtr SofaParticleInteraction<Rigid3dTypes>::createInteractionForceField() const
{
	typedef sofa::component::interactionforcefield::JointSpringForceField<Rigid3dTypes> JointSpringForceFieldRigid3dTypes;
	JointSpringForceFieldRigid3dTypes::SPtr interactionForceField = sofa::core::objectmodel::New<JointSpringForceFieldRigid3dTypes>(myInteractorMechanicalState, myInteractedMechanicalObject);
	sofa::component::interactionforcefield::JointSpring<Rigid3dTypes> spring(0, myParticleIndex);
	spring.setSoftStiffnessTranslation(myStiffness);

	interactionForceField->setName("Spring");
	interactionForceField->addSpring(spring);

	return interactionForceField;
}

}

}
