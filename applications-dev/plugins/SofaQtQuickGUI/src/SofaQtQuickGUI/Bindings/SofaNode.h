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

namespace sofaqtquick
{

namespace bindings
{

namespace _sofanode_
{

using sofaqtquick::bindings::SofaBase;
using sofa::simulation::graph::DAGNode;

/// QtQuick wrapper for a sofa base allowing us to share a component in a QML context
class SOFA_SOFAQTQUICKGUI_API SofaNode : public SofaBase
{
    //Q_OBJECT
public:
    SofaNode(QObject *parent = nullptr);
    SofaNode(DAGNode::SPtr self, QObject *parent = nullptr);
    ~SofaNode() override;

    DAGNode* self(){ return dynamic_cast<DAGNode*>(m_self.get()); } ///< Hold are reference to the real sofa object.
};

} /// namespace _sofanode_

using _sofanode_::SofaNode;

} /// namespace binding

} /// namespace sofaqtquick
