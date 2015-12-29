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

#include "SofaParticleInteractor.h"
#include "SofaScene.h"
#include "SofaViewer.h"
#include "Manipulator.h"

#include <sofa/core/visual/VisualParams.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/CleanupVisitor.h>
#include <sofa/simulation/common/DeleteVisitor.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <SofaBoundaryCondition/FixedConstraint.h>
#include <SofaDeformable/StiffSpringForceField.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <sofa/helper/cast.h>

#include <qqml.h>
#include <QDebug>

namespace sofa
{

typedef sofa::simulation::Node Node;
typedef sofa::core::visual::DisplayFlags DisplayFlags;
typedef sofa::core::visual::VisualParams VisualParams;
typedef sofa::core::visual::VisualModel VisualModel;
typedef sofa::component::visualmodel::VisualStyle VisualStyle;
typedef sofa::component::container::MechanicalObject<sofa::defaulttype::Vec3Types> MechanicalObject;
typedef sofa::component::projectiveconstraintset::FixedConstraint<sofa::defaulttype::Vec3Types> FixedConstraint;
typedef sofa::component::interactionforcefield::StiffSpringForceField<sofa::defaulttype::Vec3Types> StiffSpringForceField;

namespace core
{

namespace visual
{

class StiffSpringForcefieldPainter : public VisualModel
{
public:
    SOFA_CLASS(StiffSpringForcefieldPainter, VisualModel);

    StiffSpringForcefieldPainter(StiffSpringForceField* forcefield) : VisualModel(),
        myForcefield(forcefield)
    {

    }

    virtual void drawVisual(const VisualParams* vparams)
    {
        const DisplayFlags backupDisplayFlags = vparams->displayFlags();

        DisplayFlags displayFlags;
        displayFlags.setShowAll(true);
        const_cast<VisualParams*>(vparams)->displayFlags() = displayFlags;

        if(myForcefield)
            myForcefield->draw(vparams);

        const_cast<VisualParams*>(vparams)->displayFlags() = backupDisplayFlags;
    }

private:
    StiffSpringForceField* myForcefield;

};

} // namespace visual

} // namespace core

typedef sofa::core::visual::StiffSpringForcefieldPainter StiffSpringForcefieldPainter;

namespace qtquick
{

SofaParticleInteractor::SofaParticleInteractor(QObject *parent) : QObject(parent),
    mySofaComponent(nullptr),
    myParticleIndex(-1),
    myStiffness(100),
    myNode(nullptr),
    myMechanicalState(nullptr),
    myForcefield(nullptr)
{

}

SofaParticleInteractor::~SofaParticleInteractor()
{
	release();
}

QVector3D SofaParticleInteractor::particlePosition() const
{
    QVector3D particlePosition(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
    if(!mySofaComponent)
        return particlePosition;

    MechanicalObject* mechanicalObject = static_cast<MechanicalObject*>(myMechanicalState);
    sofa::defaulttype::Vector3 position = mechanicalObject->readPositions()[0];

    return QVector3D(position.x(), position.y(), position.z());
}

QVector3D SofaParticleInteractor::interactorPosition() const
{
    QVector3D interactorPosition(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
	if(!myMechanicalState)
        return interactorPosition;

    MechanicalObject* mechanicalObject = static_cast<MechanicalObject*>(myMechanicalState);
	sofa::defaulttype::Vector3 position = mechanicalObject->readPositions()[0];

	return QVector3D(position.x(), position.y(), position.z());
}

bool SofaParticleInteractor::interacting() const
{
    return mySofaComponent && mySofaComponent->base() && -1 != myParticleIndex;
}

bool SofaParticleInteractor::start(SofaComponent* sofaComponent, int particleIndex)
{
    release();

    if(-1 == particleIndex)
        return false;

    if(!sofaComponent)
        return false;

    const SofaScene* sofaScene = sofaComponent->sofaScene();
    MechanicalObject* particleMechanicalObject = dynamic_cast<MechanicalObject*>(sofaComponent->base());

    if(!sofaScene || !particleMechanicalObject)
        return false;

    mySofaComponent = new SofaComponent(*sofaComponent);
    myParticleIndex = particleIndex;

    QVector3D position = QVector3D(particleMechanicalObject->getPX(myParticleIndex),
                                   particleMechanicalObject->getPY(myParticleIndex),
                                   particleMechanicalObject->getPZ(myParticleIndex));

    VisualStyle::SPtr visualStyle = sofa::core::objectmodel::New<VisualStyle>();
    DisplayFlags displayFlags;
    displayFlags.setShowAll(true);
    visualStyle->displayFlags.setValue(displayFlags);

    MechanicalObject::SPtr mechanicalObject = sofa::core::objectmodel::New<MechanicalObject>();
    mechanicalObject->setName("Attractor");
    mechanicalObject->resize(1);
    mechanicalObject->writePositions()[0] = sofa::defaulttype::Vector3(position.x(), position.y(), position.z());
    myMechanicalState = mechanicalObject.get();

    FixedConstraint::SPtr fixedConstraint = sofa::core::objectmodel::New<FixedConstraint>();

    StiffSpringForceField::SPtr stiffSpringForcefield = sofa::core::objectmodel::New<StiffSpringForceField>(mechanicalObject.get(), particleMechanicalObject);
    stiffSpringForcefield->setName("Spring");
    stiffSpringForcefield->setDrawMode(0);
    stiffSpringForcefield->addSpring(0, myParticleIndex, myStiffness, 0.1, 0.0);
    myForcefield = stiffSpringForcefield.get();

    StiffSpringForcefieldPainter::SPtr forcefieldPainter = sofa::core::objectmodel::New<StiffSpringForcefieldPainter>(stiffSpringForcefield.get());

    Node::SPtr node = sofaScene->sofaSimulation()->GetRoot()->createChild("Interactor");
    node->addObject(visualStyle);
    node->addObject(mechanicalObject);
    node->addObject(fixedConstraint);
    node->addObject(stiffSpringForcefield);
    node->addObject(forcefieldPainter);
    node->init(sofa::core::ExecParams::defaultInstance());
    myNode = node.get();

    Node* particleNode = down_cast<Node>(stiffSpringForcefield->getMState2()->getContext());
    particleNode->moveObject(stiffSpringForcefield);

    interactorPositionChanged(position);
    interactingChanged(true);

    return true;
}

bool SofaParticleInteractor::update(const QVector3D& position)
{
    if(!myMechanicalState)
        return false;

    MechanicalObject* mechanicalObject = static_cast<MechanicalObject*>(myMechanicalState);
    mechanicalObject->writePositions()[0] = sofa::defaulttype::Vector3(position.x(), position.y(), position.z());
    interactorPositionChanged(position);

    return true;
}

void SofaParticleInteractor::release()
{
    if(myNode)
    {
        StiffSpringForceField::SPtr stiffSpringForcefield = static_cast<StiffSpringForceField*>(myForcefield);
        myNode->moveObject(stiffSpringForcefield);

        Node::SPtr node = static_cast<Node*>(myNode);
        node->detachFromGraph();
        node->execute<sofa::simulation::CleanupVisitor>(sofa::core::ExecParams::defaultInstance());
        node->execute<sofa::simulation::DeleteVisitor>(sofa::core::ExecParams::defaultInstance());
    }

    myNode = nullptr;
    myMechanicalState = nullptr;
    myForcefield = nullptr;

    myParticleIndex = -1;

    delete mySofaComponent;
    mySofaComponent = nullptr;

    interactingChanged(false);
}

}

}
