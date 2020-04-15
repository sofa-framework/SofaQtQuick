/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/
#include "SofaBaseObject.h"
#include "SofaNode.h"

namespace sofaqtquick::bindings::_sofaobject_
{

SofaBaseObject::SofaBaseObject(BaseObject::SPtr self, QObject *parent)
{
    SOFA_UNUSED(parent);
    m_self = self;
}

SofaBaseObject::~SofaBaseObject(){}


/// Initialization method called at graph creation and modification, during top-down traversal.
void SofaBaseObject::init()
{
    self()->init();
}

/// Initialization method called at graph creation and modification, during bottom-up traversal.
void SofaBaseObject::bwdInit()
{
    self()->bwdInit();
}

/// Update method called when variables used in precomputation are modified.
void SofaBaseObject::reinit()
{
    self()->reinit();
}

/// Save the initial state for later uses in reset()
void SofaBaseObject::storeResetState()
{
    self()->storeResetState();
}

/// Reset to initial state
void SofaBaseObject::reset()
{
    self()->reset();
}

/// Called just before deleting this object
/// Any object in the tree bellow this object that are to be removed will be removed only after this call,
/// so any references this object holds should still be valid.
void SofaBaseObject::cleanup()
{
    self()->cleanup();
}

SofaNode* SofaBaseObject::getFirstParent()
{
    return SofaNode::createFrom(self()->getContext());
}

bool SofaBaseObject::rename(const QString& name)
{
    auto obj = dynamic_cast<sofa::simulation::Node*>(getFirstParent()->self())->getObject(name.toStdString());
    if (obj != nullptr && obj->getPathName() != m_self->getPathName())
        return false;
    m_self->setName(name.toStdString());
    return true;
}

unsigned int SofaBaseObjectList::size()
{
    return unsigned(m_list.size());
}

SofaBaseObject* SofaBaseObjectList::at(unsigned int n)
{
    return m_list.at(n);
}

SofaBaseObject* SofaBaseObjectList::first()
{
    return m_list.front();
}

SofaBaseObject* SofaBaseObjectList::last()
{
    return m_list.back();
}

void SofaBaseObjectList::addSofaBaseObject(BaseObject* obj)
{
    m_list.push_back(new SofaBaseObject(BaseObject::SPtr(obj)));
}

}  /// namespace sofaqtquick::bindings::_sofaobject_

