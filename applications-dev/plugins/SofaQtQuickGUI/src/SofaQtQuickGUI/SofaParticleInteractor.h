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

#ifndef SOFAPARTICLEINTERACTOR_H
#define SOFAPARTICLEINTERACTOR_H

#include <QObject>
#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>

#include <sofa/core/visual/VisualParams.h>
#include <SofaBaseMechanics/MechanicalObject.h>

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

namespace visual
{

class InteractionForceFieldPainter : public VisualModel
{
public:
    SOFA_CLASS(InteractionForceFieldPainter, VisualModel);

    InteractionForceFieldPainter(sofa::core::behavior::BaseInteractionForceField* forcefield) : VisualModel(),
        myForcefield(forcefield)
    {

    }

    virtual void drawVisual(const VisualParams* vparams) override
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
    sofa::core::behavior::BaseInteractionForceField* myForcefield;

};

} // namespace visual

} // namespace core

namespace qtquick
{

class SofaComponent;
class SofaScene;
class Manipulator;

class SofaBaseParticleInteraction
{
protected:
	SofaBaseParticleInteraction() {}

public:
	virtual ~SofaBaseParticleInteraction() {}

	virtual void start() = 0;
	virtual void update(const QVector3D& position) = 0;
	virtual void release() = 0;

	virtual QVector3D position() const = 0;
	virtual SofaComponent* sofaComponent() const = 0;
	virtual int particleIndex() const = 0;

};

template<class Types>
class SofaParticleInteraction : public SofaBaseParticleInteraction
{
public:
	SofaParticleInteraction<Types>(const SofaComponent* sofaComponent, int particleIndex, double stiffness) : SofaBaseParticleInteraction(),
		mySofaComponent(new SofaComponent(*sofaComponent)),
		myParticleIndex(particleIndex),
		myStiffness(stiffness)
	{
		myInteractedMechanicalObject = dynamic_cast<MechanicalObject*>(mySofaComponent->base());
	}

public:
    typedef sofa::simulation::Node Node;
	typedef sofa::component::container::MechanicalObject<Types> MechanicalObject;

	virtual void start();
	virtual void update(const QVector3D& position);
	virtual void release();

	virtual QVector3D position() const;
	virtual SofaComponent* sofaComponent() const;
	virtual int particleIndex() const;

private:
    sofa::core::behavior::BaseInteractionForceField::SPtr createInteractionForceField() const;

private:
	SofaComponent*										mySofaComponent;
	int													myParticleIndex;
	double												myStiffness;

	MechanicalObject*									myInteractedMechanicalObject;
	MechanicalObject*									myInteractorMechanicalState;
	sofa::core::objectmodel::BaseNode*					myNode;
	sofa::core::behavior::BaseInteractionForceField*	myInteractionForceField;

};

/// \class Allow us to interact with a sofa particle, typically a degree of freedom (dof) in a sofa MechanicalObject
class SOFA_SOFAQTQUICKGUI_API SofaParticleInteractor : public QObject
{
    Q_OBJECT

public:
    SofaParticleInteractor(QObject *parent = 0);
    ~SofaParticleInteractor();
	
public:
    Q_PROPERTY(double stiffness READ stiffness WRITE setStiffness NOTIFY stiffnessChanged)
    Q_PROPERTY(QVector3D interactorPosition READ interactorPosition NOTIFY interactorPositionChanged)
    Q_PROPERTY(bool interacting READ interacting NOTIFY interactingChanged)

public:
	double stiffness() const;
	void setStiffness(double stiffness);

    QVector3D interactorPosition() const;
    bool interacting() const;

	Q_INVOKABLE sofa::qtquick::SofaComponent* sofaComponent() const;

signals:
	void stiffnessChanged(double);
    void interactorPositionChanged(const QVector3D&);
    void interactingChanged(bool);

public slots:
    bool start(SofaComponent* sofaComponent, int particleIndex);
    bool update(const QVector3D& interactorNewPosition);
    void release();

private:
    double									myStiffness;

	SofaBaseParticleInteraction*			myParticleInteraction;

};

}

}

#endif // SOFAPARTICLEINTERACTOR_H
