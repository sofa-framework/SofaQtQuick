#include <SofaQtQuickGUI/config.h>
#include "SofaBase.h"

#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject;

namespace sofaqtquick::bindings::_sofaobject_
{

using sofaqtquick::bindings::SofaBase;

/// QtQuick wrapper to sofa::Node
class SOFA_SOFAQTQUICKGUI_API SofaBaseObject: public SofaBase
{
public:
    SofaBaseObject(BaseObject::SPtr self, QObject *parent = nullptr);
    ~SofaBaseObject() override;

    BaseObject* self(){ return dynamic_cast<BaseObject*>(m_self.get()); } ///< Hold are reference to the real sofa object.
    BaseObject::SPtr selfptr(){ return BaseObject::SPtr(dynamic_cast<BaseObject*>(m_self.get())); } ///< Hold are reference to the real sofa object.
};


}  // namespace sofaqtquick::bindings::_sofaobject_
