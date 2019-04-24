#pragma once

#include <SofaQtQuickGUI/config.h>
#include "SofaBase.h"

#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject;

namespace sofaqtquick::bindings
{
namespace _sofaobject_
{

using sofaqtquick::bindings::SofaBase;

/// QtQuick wrapper to sofa::Node
class SOFA_SOFAQTQUICKGUI_API SofaBaseObject: public SofaBase
{
    Q_OBJECT

public:
    SofaBaseObject(QObject *parent = nullptr) { SOFA_UNUSED(parent); m_self = nullptr; }
    SofaBaseObject(BaseObject::SPtr self, QObject *parent = nullptr);
    ~SofaBaseObject() override;

    BaseObject* self(){ return dynamic_cast<BaseObject*>(m_self.get()); } ///< Hold are reference to the real sofa object.
    BaseObject::SPtr selfptr(){ return BaseObject::SPtr(dynamic_cast<BaseObject*>(m_self.get())); } ///< Hold are reference to the real sofa object.


    /// Initialization method called at graph creation and modification, during top-down traversal.
    Q_INVOKABLE void init();

    /// Initialization method called at graph creation and modification, during bottom-up traversal.
    Q_INVOKABLE void bwdInit();

    /// Update method called when variables used in precomputation are modified.
    Q_INVOKABLE void reinit();

    /// Save the initial state for later uses in reset()
    Q_INVOKABLE void storeResetState();

    /// Reset to initial state
    Q_INVOKABLE void reset();

    /// Called just before deleting this object
    /// Any object in the tree bellow this object that are to be removed will be removed only after this call,
    /// so any references this object holds should still be valid.
    Q_INVOKABLE void cleanup();

};

}  // namespace _sofaobject_

using _sofaobject_::SofaBaseObject;

}  // namespace sofaqtquick::bindings
