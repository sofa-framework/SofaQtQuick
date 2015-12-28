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

#include "SofaQtQuickGUI.h"
#include "SofaViewer.h"
#include "SofaComponent.h"

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

class SofaComponent;
class SofaScene;
class Manipulator;

/// \class Allow us to interact with a sofa particle, typically a degree of freedom (dof) in a sofa MechanicalObject
class SOFA_SOFAQTQUICKGUI_API SofaParticleInteractor : public QObject
{
    Q_OBJECT

	typedef sofa::core::behavior::BaseMechanicalState			BaseMechanicalState;
	typedef sofa::core::behavior::BaseInteractionForceField		BaseInteractionForceField;
	typedef sofa::core::objectmodel::BaseNode					BaseNode;
    typedef sofa::component::visualmodel::OglModel              OglModel;

public:
    SofaParticleInteractor(QObject *parent = 0);
    ~SofaParticleInteractor();
	
public:
    Q_PROPERTY(sofa::qtquick::SofaComponent* sofaComponent MEMBER mySofaComponent NOTIFY sofaComponentChanged)
    Q_PROPERTY(int particleIndex MEMBER myParticleIndex NOTIFY particleIndexChanged)
    Q_PROPERTY(double stiffness MEMBER myStiffness NOTIFY stiffnessChanged)
    Q_PROPERTY(QVector3D interactorPosition READ interactorPosition NOTIFY interactorPositionChanged)
    Q_PROPERTY(bool interacting READ interacting NOTIFY interactingChanged)

public:
    SofaComponent* sofaComponent() const        {return mySofaComponent;}
    double stiffness() const                    {return myStiffness;}
    QVector3D interactorPosition() const;
    bool interacting() const;

signals:
    void sofaComponentChanged(sofa::qtquick::SofaComponent* newSofaComponent);
    void particleIndexChanged(double newParticleIndex);
	void stiffnessChanged(double newStiffness);
    void interactorPositionChanged(const QVector3D& newInteractorPosition);
    void interactingChanged(bool newInteracting);

public:
    Q_INVOKABLE QVector3D particlePosition() const;
	
public slots:
    bool start(SofaComponent* sofaComponent, int particleIndex);
    bool update(const QVector3D& interactorNewPosition);
    void release();

private:
    SofaComponent*                         mySofaComponent;
    int                                     myParticleIndex;
    double									myStiffness;

	BaseNode*								myNode;
	BaseMechanicalState*					myMechanicalState;
    BaseInteractionForceField*				myForcefield;
};

}

}

#endif // SOFAPARTICLEINTERACTOR_H
