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
#include <sofa/core/objectmodel/Base.h>
#include <sofa/core/objectmodel/BaseData.h>

namespace sofaqtquick
{

namespace bindings
{

namespace _sofabase_
{

using sofa::core::objectmodel::Base;
using sofa::core::objectmodel::BaseData;

/// QtQuick wrapper for a sofa base allowing us to share a component in a QML context
class SOFA_SOFAQTQUICKGUI_API SofaBase : public QObject
{
    Q_OBJECT
public:
    SofaBase(Base::SPtr self);
    ~SofaBase() {}

    Q_INVOKABLE QString getName() const;
    Q_INVOKABLE QString getClassName() const;
    Q_INVOKABLE QString getTemplateName() const;

    Q_INVOKABLE QObject* getData(const QString& name) const;

protected:
    SofaBase();

    Base::SPtr m_self; ///< Hold are reference to the real sofa object.
};


//SP_CLASS_METHOD_DOC(Base,addData, "Adds an existing data field to the current object")
//SP_CLASS_METHOD_DOC(Base,findData, "Returns the data field if there is one associated \n"
//                                   "with the provided name and downcasts it to the lowest known type. \n"
//                                   "Returns None otherwhise.")
//SP_CLASS_METHOD_DOC(Base,findLink, "Returns a link field if there is one associated \n"
//                                   "with the provided name, returns None otherwhise")
//SP_CLASS_METHOD_DOC(Base,getData, "Returns the data field if there is one associated \n"
//                              "with the provided name but don't downcasts it to the lowest known type. \n"
//                              "Returns None is there is no field with this name.")
//SP_CLASS_METHOD_DOC(Base,getLink, "Returns the link field if there is one associated \n"
//                              "with the provided name but. \n"
//                              "Returns None is there is no field with this name.")
//SP_CLASS_METHOD(Base,__dir__)
//SP_CLASS_METHOD_DOC(Base,getDataFields, "Returns a list with the *content* of all the data fields converted in python"
//                                        " type. \n")
//SP_CLASS_METHOD_DOC(Base,getListOfDataFields, "Returns the list of data fields.")
//SP_CLASS_METHOD_DOC(Base,getListOfLinks, "Returns the list of link fields.")
//SP_CLASS_METHOD(Base,downCast)

//SP_CLASS_METHODS_END;


} /// namespace _sofabase_

using _sofabase_::SofaBase;

} /// namespace binding

} /// namespace sofaqtquick
