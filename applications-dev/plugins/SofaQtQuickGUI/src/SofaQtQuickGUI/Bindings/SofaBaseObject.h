#pragma once

#include <SofaQtQuickGUI/config.h>

#include <sofa/core/objectmodel/BaseObject.h>

#include "SofaBase.h"

namespace sofaqtquick::bindings
{

namespace _sofanode_
{
    class SofaNode;
}
using _sofanode_::SofaNode;

namespace _sofaobject_
{
using sofa::core::objectmodel::BaseObject;
using sofaqtquick::bindings::SofaBase;
using sofaqtquick::bindings::SofaNode;

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

    Q_INVOKABLE SofaNode* getFirstParent();

    Q_INVOKABLE bool rename(const QString& name) override;
};

class SofaBaseObjectList : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE unsigned int size();
    Q_INVOKABLE SofaBaseObject* at(unsigned int);
    Q_INVOKABLE SofaBaseObject* first();
    Q_INVOKABLE SofaBaseObject* last();
    Q_INVOKABLE void append(SofaBaseObject* sofaBaseObject) { m_list.push_back(sofaBaseObject); }

    void addSofaBaseObject(sofa::core::objectmodel::BaseObject* obj);

private:
    std::vector<SofaBaseObject*> m_list;
};


}  // namespace _sofaobject_

using _sofaobject_::SofaBaseObject;
using _sofaobject_::SofaBaseObjectList;
}  // namespace sofaqtquick::bindings
