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

#include "SofaCompliantInteractor.h"
#include <SofaQtQuickGUI/SofaScene.h>
// #include "SofaViewer.h"
// #include "Manipulator.h"

#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/visual/VisualModel.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/simulation/MechanicalVisitor.h>
#include <sofa/simulation/CleanupVisitor.h>
#include <sofa/simulation/DeleteVisitor.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <SofaBoundaryCondition/FixedConstraint.h>
#include <SofaRigid/JointSpringForceField.h>
#include <SofaBaseVisual/VisualStyle.h>
#include <sofa/helper/cast.h>
#include <sofa/core/ObjectFactory.h>


#include <Compliant/mapping/DifferenceFromTargetMapping.h>
#include <Compliant/compliance/UniformCompliance.h>

#include <typeindex>

#include <qqml.h>
#include <QDebug>

#include <SofaQtQuickGUI/SofaApplication.h>
#include <QWindow>
#include <QTimer>

#include <sofa/helper/cast.h>

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

SofaCompliantInteractor::SofaCompliantInteractor(QObject *parent)
: QObject(parent)
{
    setObjectName("compliant-interactor");
}

SofaCompliantInteractor::~SofaCompliantInteractor()
{ 
}

void SofaCompliantInteractor::set(double _compliance,
                                  bool _isCompliance,
                                  float _arrowSize,
                                  const QColor& _color,
                                  bool _visualmodel)
{
    compliance=_compliance;
    isCompliance=_isCompliance;
    arrowSize=_arrowSize;
    color=_color;
    visualmodel=_visualmodel;
}


template<class Types>
std::function< bool(const QVector3D&) > SofaCompliantInteractor::update_thunk(SofaComponent* component,
									      int index) {
    
//    std::clog << "SofaCompliantInteractor::update_thunk" << std::endl;

    using state_type = MechanicalObject<Types>;

    // interactor dofs
    auto state = dynamic_cast<state_type*>(component->base());
    assert(state && "bad state type in interactor thunk");
    
    using namespace component;
    using core::objectmodel::New;
    using helper::write;
    
    // difference dofs
    auto dofs = New<container::MechanicalObject<Types>>();
    
    // difference mapping
    auto mapping = New<mapping::DifferenceFromTargetMapping<Types, Types>>();
    mapping->setFrom(state);
    mapping->setTo(dofs.get());

    write(mapping->indices).wref().resize(1);
    write(mapping->indices).wref()[0] = index;
    
    write(mapping->targets).wref().resize(1);
    write(mapping->targets).wref()[0] = write(state->x).wref()[index];

    mapping->d_showObjectScale.setValue( arrowSize );
    mapping->d_color.setValue( defaulttype::Vec4f( color.red()/255.0, color.green()/255.0, color.blue()/255.0, color.alpha()/255.0) );


    
    // forcefield on difference
    auto ff = New<forcefield::UniformCompliance<Types>>();
    
    ff->compliance.setValue(compliance);
    
    ff->damping.setValue(1.0 / (1.0 + compliance) );

    // display flags
    auto style = New<visualmodel::VisualStyle>();
    helper::write(style->displayFlags).wref().setShowMechanicalMappings(true);
    
    // node setup
    node = down_cast<simulation::Node>(state->getContext())->createChild("Interactor");
    
    node->addObject(dofs);
    node->addObject(mapping);
    node->addObject(ff);
    node->addObject(style);
    
    node->init( core::ExecParams::defaultInstance() );

    // interactor position update
    position = QVector3D(state->getPX(index), 
			 state->getPY(index),
			 state->getPZ(index));
    
    return [mapping,this](const QVector3D& pos) -> bool {
	
	position = pos;
	write(mapping->targets).wref()[0] = {pos.x(), pos.y(), pos.z()};
	
	return true;
    };
}

bool SofaCompliantInteractor::start(SofaComponent* sofaComponent, int particleIndex)
{

    using thunk_type = update_cb_type (SofaCompliantInteractor::*)(SofaComponent* component,
								   int index);

    // double dispatch table
    // TODO rigid interactors and friends
    static std::map< std::type_index, thunk_type> dispatch {
#ifndef SOFA_FLOAT
	{std::type_index(typeid(MechanicalObject<Vec3dTypes>)), &SofaCompliantInteractor::update_thunk<Vec3dTypes>},
#endif
#ifndef SOFA_DOUBLE
	{std::type_index(typeid(MechanicalObject<Vec3fTypes>)), &SofaCompliantInteractor::update_thunk<Vec3fTypes>}
#endif
    };
    
    std::type_index key = typeid(*sofaComponent->base());
    auto it = dispatch.find(key);

    if(it == dispatch.end()) {
	return false;
    }

    update_cb = (this->*it->second)(sofaComponent, particleIndex);
    return true;
}

bool SofaCompliantInteractor::update(const QVector3D& position)
{
//    std::clog << "SofaCompliantInteractor::update" << std::endl;
    if(update_cb) return update_cb(position);
    else return false;
}

bool SofaCompliantInteractor::interacting() const {
    return bool(update_cb);
}
    
void SofaCompliantInteractor::release()
{
//     std::clog << "SofaCompliantInteractor::release" << std::endl;
    update_cb = 0;

    if( node ) {
	node->detachFromGraph();
	node = 0;
    }
}

void SofaCompliantInteractor::set_compliant_interactor( double compliance,
                                                        bool isCompliance,
                                                        float arrowSize,
                                                        const QColor& color,
                                                        bool visualmodel )
{
    // a crude hack to make sure we set property in the event loop
    // thread
    QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [compliance, isCompliance, arrowSize, color, visualmodel, timer] {
    foreach(QObject* obj, QGuiApplication::allWindows() ) {
      auto scene = obj->findChild<sofa::qtquick::SofaScene*>();
      if(scene) {
        QVariant var;
        auto* interactor = sofa::qtquick::SofaCompliantInteractor::getInstance();
        interactor->set( compliance, isCompliance, arrowSize, color, visualmodel );
        var.setValue(interactor);
        scene->setProperty("sofaParticleInteractor", var);
        break;
      }
    }
    // also because we're nice people
    timer->deleteLater();
  });

    timer->setSingleShot(true);
    timer->start(0);
}



} // namespace qtquick


} // namespace sofa




extern "C" {

    // todo: update to take all parameters into account
    // or even better: remove it!
    void set_compliant_interactor(double compliance) {
        sofa::qtquick::SofaCompliantInteractor::set_compliant_interactor( compliance );
    }
  
}



#include <Compliant/misc/CompliantAttachButtonSetting.h>

// RAII to overloaded CompliantAttachButtonSetting::init
// to be able to create a compliant mouse in qtquicksofa
// from the CompliantAttachButtonSetting component
static struct InitCompliantAttachButtonSetting
{
    InitCompliantAttachButtonSetting()
    {
        sofa::component::configurationsetting::CompliantAttachButtonSetting::s_initFunction   = InitCompliantAttachButtonSetting::initCompliantAttachButtonSetting;
        sofa::component::configurationsetting::CompliantAttachButtonSetting::s_reinitFunction = InitCompliantAttachButtonSetting::reinitCompliantAttachButtonSetting;
    }

    ~InitCompliantAttachButtonSetting()
    {
        sofa::component::configurationsetting::CompliantAttachButtonSetting::s_initFunction = nullptr;
    }

    static void initCompliantAttachButtonSetting( sofa::component::configurationsetting::CompliantAttachButtonSetting* _this )
    {
        const sofa::defaulttype::Vec4f& color = _this->color.getValue();
        sofa::qtquick::SofaCompliantInteractor::set_compliant_interactor(_this->compliance.getValue(),
                                                                         _this->isCompliance.getValue(),
                                                                         _this->arrowSize.getValue(),
                                                                         QColor( color[0]*255, color[1]*255, color[2]*255, color[3]*255 ),
                                                                         _this->visualmodel.getValue() );
    }

    static void reinitCompliantAttachButtonSetting( sofa::component::configurationsetting::CompliantAttachButtonSetting* _this )
    {
        const sofa::defaulttype::Vec4f& color = _this->color.getValue();
        sofa::qtquick::SofaCompliantInteractor::getInstance()->set(_this->compliance.getValue(),
                                                                         _this->isCompliance.getValue(),
                                                                         _this->arrowSize.getValue(),
                                                                         QColor( color[0]*255, color[1]*255, color[2]*255, color[3]*255 ),
                                                                         _this->visualmodel.getValue() );
    }

} initCompliantAttachButtonSetting;
