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

#ifndef SOFA_COMPLIANT_INTERACTOR_H
#define SOFA_COMPLIANT_INTERACTOR_H

#include <QObject>

#include "CompliantQtQuickGUI.h"

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
// #include "SofaViewer.h"
#include <SofaQtQuickGUI/SofaComponent.h>

#include <sofa/core/visual/VisualParams.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include "CompliantQtQuickGUI.h"

#include <QObject>
#include <QVector3D>

#include <functional>

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

class CompliantPainter : public VisualModel
{
public:
    SOFA_CLASS(CompliantPainter, VisualModel);

    CompliantPainter();
    virtual void drawVisual(const VisualParams* vparams);

private:

};

} // namespace visual

} // namespace core

namespace qtquick
{

class SofaComponent;
class SofaScene;
class Manipulator;


/// \class Allow us to interact with a sofa particle, typically a
/// degree of freedom (dof) in a sofa MechanicalObject
class SOFA_COMPLIANT_QTQUICKGUI_API SofaCompliantInteractor : public QObject
{
    Q_OBJECT

public:
    SofaCompliantInteractor(double compliance = 1, QObject *parent = 0);
    
    ~SofaCompliantInteractor();
    
public:
    Q_PROPERTY(double compliance MEMBER compliance NOTIFY complianceChanged)
    Q_PROPERTY(QVector3D interactorPosition MEMBER position NOTIFY interactorPositionChanged)
    Q_PROPERTY(bool interacting READ interacting NOTIFY interactingChanged)

public:
    
    bool interacting() const;
    
    // Q_INVOKABLE sofa::qtquick::SofaComponent* sofaComponent() const;

signals:
    
    void complianceChanged(double);
    void interactingChanged(bool);
    
    void interactorPositionChanged(const QVector3D&);


public slots:
    bool start(sofa::qtquick::SofaComponent* sofaComponent, int particleIndex);
    bool update(const QVector3D& pos);
    void release();

private:
    double compliance;

    sofa::simulation::Node::SPtr node;

    using update_cb_type = std::function< bool(const QVector3D&) >;
    update_cb_type update_cb;

    QVector3D position;
    
    template<class Types>
    update_cb_type update_thunk(SofaComponent* base, int particle_index);
};

}

}

#endif // SOFAPARTICLEINTERACTOR_H
