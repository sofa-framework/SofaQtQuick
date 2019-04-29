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
#pragma once

#include <QObject>

#include <SofaQtQuickGUI/config.h>

#include <SofaSimulationGraph/DAGNode.h>
#include "SofaBase.h"
#include "SofaBaseObject.h"

namespace sofa::qtquick
{
class SofaComponent;
class SofaScene;
}  // namespace sofa::qtquick

namespace sofaqtquick::bindings
{

namespace _sofanode_
{

using sofaqtquick::bindings::SofaBase;
using sofaqtquick::bindings::_sofaobject_::SofaBaseObject;
using sofa::simulation::graph::DAGNode;

/// QtQuick wrapper to sofa::Node
class SOFA_SOFAQTQUICKGUI_API SofaNode : public SofaBase
{
public:
    SofaNode(QObject *parent = nullptr);
    SofaNode(DAGNode::SPtr self, QObject *parent = nullptr);
    ~SofaNode() override;

    DAGNode* self(){ return dynamic_cast<DAGNode*>(m_self.get()); } ///< Hold are reference to the real sofa object.
    DAGNode::SPtr selfptr(){ return DAGNode::SPtr(dynamic_cast<DAGNode*>(m_self.get())); } ///< Hold are reference to the real sofa object.

    sofa::qtquick::SofaComponent* toSofaComponent(sofa::qtquick::SofaScene* scene = nullptr); ///< legacy method to return a deprecated SofaComponent class

    Q_INVOKABLE SofaNode* createChild(QString name);

    Q_INVOKABLE SofaNode* getFirstParent();
    Q_INVOKABLE SofaNode* getChild(QString name);
    Q_INVOKABLE SofaNode* getNodeInGraph(QString name);
    Q_INVOKABLE SofaNode* getRoot();


    Q_INVOKABLE void addChild(SofaNode* child);
    Q_INVOKABLE void addObject(SofaBaseObject* obj);
};

} /// namespace _sofanode_

using _sofanode_::SofaNode;

} /// namespace sofaqtquick::bindings
