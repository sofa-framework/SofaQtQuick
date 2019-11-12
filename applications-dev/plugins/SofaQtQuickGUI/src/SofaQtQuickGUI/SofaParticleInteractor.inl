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

#include <SofaQtQuickGUI/SofaParticleInteractor.h>
#include <SofaQtQuickGUI/SofaBaseScene.h>
#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator.h>

#include <sofa/core/visual/VisualModel.h>
#include <sofa/simulation/MechanicalVisitor.h>
#include <sofa/simulation/CleanupVisitor.h>
#include <sofa/simulation/DeleteVisitor.h>
#include <SofaBoundaryCondition/FixedConstraint.h>
#include <SofaDeformable/StiffSpringForceField.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <sofa/helper/cast.h>

#include <qqml.h>
#include <QDebug>

namespace sofaqtquick
{

template<class Types>
inline void SofaParticleInteraction<Types>::start()
{
	if(!mySofaComponent->isValid())
		return;

	QVector3D position(myInteractedMechanicalObject->getPX(myParticleIndex), myInteractedMechanicalObject->getPY(myParticleIndex), myInteractedMechanicalObject->getPZ(myParticleIndex));

	typedef sofa::component::visualmodel::VisualStyle VisualStyle;
	VisualStyle::SPtr visualStyle = sofa::core::objectmodel::New<VisualStyle>();
	sofa::core::visual::DisplayFlags displayFlags;
	displayFlags.setShowAll(true);
	visualStyle->displayFlags.setValue(displayFlags);

        typename MechanicalObject::SPtr interactorMechanicalObject = sofa::core::objectmodel::New<MechanicalObject>();
	interactorMechanicalObject->setName("Attractor");
	interactorMechanicalObject->resize(1);
	interactorMechanicalObject->writePositions()[0] = sofa::defaulttype::Vector3(position.x(), position.y(), position.z());
	myInteractorMechanicalState = interactorMechanicalObject.get();

        typedef sofa::component::projectiveconstraintset::FixedConstraint<Types> FixedConstraint;
        typename FixedConstraint::SPtr fixedConstraint = sofa::core::objectmodel::New<FixedConstraint>();

	sofa::core::behavior::BaseInteractionForceField::SPtr interactionForceField = createInteractionForceField();
	myInteractionForceField = interactionForceField.get();

	typedef sofa::core::visual::InteractionForceFieldPainter InteractionForceFieldPainter;
	InteractionForceFieldPainter::SPtr forcefieldPainter = sofa::core::objectmodel::New<InteractionForceFieldPainter>(interactionForceField.get());

    Node::SPtr node = mySofaComponent->sofaScene()->sofaRootNode()->createChild("Interactor");
	node->addObject(visualStyle);
	node->addObject(interactorMechanicalObject);
	node->addObject(fixedConstraint);
	node->addObject(interactionForceField);
	node->addObject(forcefieldPainter);
	node->init(sofa::core::ExecParams::defaultInstance());
	myNode = node.get();

	Node* particleNode = down_cast<Node>(myInteractedMechanicalObject->getContext());
	particleNode->moveObject(interactionForceField);
}

template<class Types>
inline void SofaParticleInteraction<Types>::update(const QVector3D& position)
{
	MechanicalObject* mechanicalObject = static_cast<MechanicalObject*>(myInteractorMechanicalState);
	mechanicalObject->writePositions()[0] = sofa::defaulttype::Vector3(position.x(), position.y(), position.z());
}

template<class Types>
inline void SofaParticleInteraction<Types>::release()
{
	if(myNode)
	{
		myNode->moveObject(myInteractionForceField);

                Node::SPtr node = static_cast<Node*>(myNode);
		node->detachFromGraph();
		node->execute<sofa::simulation::CleanupVisitor>(sofa::core::ExecParams::defaultInstance());
		node->execute<sofa::simulation::DeleteVisitor>(sofa::core::ExecParams::defaultInstance());
	}

	myNode = nullptr;
	myInteractorMechanicalState = nullptr;
	myInteractionForceField = nullptr;

	myParticleIndex = -1;

	delete mySofaComponent;
	mySofaComponent = nullptr;
}

template<class Types>
inline QVector3D SofaParticleInteraction<Types>::position() const
{
	return QVector3D(myInteractorMechanicalState->getPX(0), myInteractorMechanicalState->getPY(0), myInteractorMechanicalState->getPZ(0));
}

template<class Types>
inline sofa::qtquick::SofaComponent* SofaParticleInteraction<Types>::sofaComponent() const
{
	return mySofaComponent;
}

template<class Types>
inline int SofaParticleInteraction<Types>::particleIndex() const
{
	return myParticleIndex;
}

template<class Types>
inline sofa::core::behavior::BaseInteractionForceField::SPtr SofaParticleInteraction<Types>::createInteractionForceField() const
{
	typedef sofa::component::interactionforcefield::StiffSpringForceField<Types> StiffSpringForceField;
        typename StiffSpringForceField::SPtr interactionForceField = sofa::core::objectmodel::New<StiffSpringForceField>(myInteractorMechanicalState, myInteractedMechanicalObject);
	interactionForceField->setName("Spring");
	interactionForceField->setDrawMode(0);
	interactionForceField->addSpring(0, myParticleIndex, myStiffness, 0.1, 0.0);
	return interactionForceField;
}

}  // namespace sofaqtquick
