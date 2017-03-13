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
#include <QColor>

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

protected:
    SofaCompliantInteractor(QObject *parent = 0);
    
    ~SofaCompliantInteractor();
    
public:
    Q_PROPERTY(double compliance MEMBER compliance NOTIFY complianceChanged)
    Q_PROPERTY(bool isCompliance MEMBER isCompliance NOTIFY isComplianceChanged)
    Q_PROPERTY(float arrowSize MEMBER arrowSize NOTIFY arrowSizeChanged)
    Q_PROPERTY(QColor color MEMBER color NOTIFY colorChanged)
    Q_PROPERTY(bool visualmodel MEMBER visualmodel NOTIFY visualmodelChanged)

    Q_PROPERTY(QVector3D interactorPosition MEMBER position NOTIFY interactorPositionChanged)
    Q_PROPERTY(bool interacting READ interacting NOTIFY interactingChanged)

public:

    void set(SReal compliance = 1,
             bool isCompliance = false,
             float arrowSize = 0.f,
             const QColor& color = Qt::yellow,
             bool visualmodel = false);
    
    bool interacting() const;
    
    // Q_INVOKABLE sofa::qtquick::SofaComponent* sofaComponent() const;

signals:
    
    void complianceChanged(double);
    void isComplianceChanged(bool);
    void arrowSizeChanged(float);
    void colorChanged(const QColor&);
    void visualmodelChanged(bool);


    void interactorPositionChanged(const QVector3D&);
    void interactingChanged(bool);


public slots:
    bool start(sofa::qtquick::SofaComponent* sofaComponent, int particleIndex);
    bool update(const QVector3D& pos);
    void release();

private:
    // mechanical options
    double compliance;
    bool isCompliance;

    // display options
    float arrowSize;
    QColor color;
    bool visualmodel;

    sofa::simulation::Node::SPtr node;

    using update_cb_type = std::function< bool(const QVector3D&) >;
    update_cb_type update_cb;

    QVector3D position;
    
    template<class Types>
    update_cb_type update_thunk(SofaComponent* base, int particle_index);

public:
    static void set_compliant_interactor( double compliance,
                                          bool isCompliance = false,
                                          float arrowSize = 0.f,
                                          const QColor& color = Qt::yellow,
                                          bool visualmodel = false );

    static SofaCompliantInteractor* getInstance() { static SofaCompliantInteractor sofaCompliantInteractor; return &sofaCompliantInteractor;}

};

} // namespace qtquick


} // namespace sofa

#endif // SOFAPARTICLEINTERACTOR_H
