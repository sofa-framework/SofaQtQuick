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
#include <QJSValue>
namespace sofaqtquick::bindings
{

namespace _sofalink_ { class SofaLink; }
namespace _sofadata_ { class SofaData; }
namespace _sofabase_ { class SofaBase; }
namespace _sofanode_ { class SofaNode; }
namespace _sofaobject_ { class SofaBaseObject; }

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
    SofaBase(const SofaBase& o);
    ~SofaBase() {}

    Q_INVOKABLE QString getName() const;
    Q_INVOKABLE void setName(const QString& name);
    virtual bool rename(const QString& /*name*/) { return false; }
    Q_INVOKABLE QString getClassName() const;
    Q_INVOKABLE QStringList getInheritedClassNames() const;
    Q_INVOKABLE QString getTemplateName() const;
    Q_INVOKABLE QString getPathName() const;

    Q_INVOKABLE QStringList getDataFields() const;

    /// get a link from its name
    Q_INVOKABLE _sofalink_::SofaLink* findLink(const QString& name) const;
    Q_INVOKABLE QStringList getLinks() const;

    /// Returns a data if one is found with the given name,
    /// else a link is one is found with the given name,
    /// else nullptr
    /// (see python's Sofa.Core.Base.__getattr__ for more info)
    Q_INVOKABLE QObject* get(const QString& s);

    /// get a data from its name, throw an exception if not there.
    Q_INVOKABLE _sofadata_::SofaData* getData(const QString& name) const;
    /// find a data from its name, returns null if not found.
    Q_INVOKABLE _sofadata_::SofaData* findData(const QString& name) const;




    /// Returns true if the underlying Base is a Node.
    Q_INVOKABLE bool isNode() const;
    bool isSame(SofaBase* sofaComponent) const;

    /// The following three are used to get extra information about where
    /// the component is implemented (the source location)
    /// and where it is instanciated (the scene file location)
    /// If the informations are available it returns
    /// a string containing ["/path/to/file", 19]
    /// In case there is no filename the method returns the empty string
    Q_INVOKABLE bool hasLocations() const ;
    Q_INVOKABLE QString getSourceLocation() const ;
    Q_INVOKABLE QString getInstanciationLocation() const ;

    Q_INVOKABLE bool hasMessage() const;

    Q_INVOKABLE QString output() const;
    Q_INVOKABLE void clearOutput() const;

    Q_INVOKABLE QString warning() const;
    Q_INVOKABLE void clearWarning() const;

    Q_INVOKABLE void dump() const;

    Q_INVOKABLE QString componentStateAsString() const;

    Base*      rawBase() const { return m_self.get(); }
    Base::SPtr base(){ return m_self; }

protected:
    SofaBase();

    Base::SPtr m_self {nullptr}; ///< Hold are reference to the real sofa object.
};

} /// namespace _sofabase_

using _sofabase_::SofaBase;

} /// namespace sofaqtquick::bindings
