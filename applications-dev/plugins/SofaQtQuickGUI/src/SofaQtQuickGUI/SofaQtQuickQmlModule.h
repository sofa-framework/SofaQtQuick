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

#ifndef SOFAQTQUICKSOFA_SofaQtQuickQmlModule_H
#define SOFAQTQUICKSOFA_SofaQtQuickQmlModule_H

#include <QQmlExtensionPlugin>

#include <SofaQtQuickGUI/config.h>

namespace sofaqtquick
{

/// \class Initialize the Qml Module containing the complete SofaQtQuick binding.
/// This is working as long as dynamically loading plugin is not needed.
/// If this is the case you should read:
/// More info: http://doc.qt.io/qt-5/qqmlextensionplugin.html
class SOFA_SOFAQTQUICKGUI_API SofaQtQuickQmlModule
{
public:
    /// Register the types without the need of creating an instance of the SofaQtQuickQmlModule.
    static void RegisterTypes(QQmlEngine* engine);

};

}

#endif /// SOFAQTQUICKSOFA_QMLBINDINGSOFATYPES_H
