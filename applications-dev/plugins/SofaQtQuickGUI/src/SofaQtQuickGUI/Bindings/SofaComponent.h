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

#pragma once
#include <sofa/simulation/Simulation.h>

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>

namespace sofa
{

namespace qtquick
{


class SofaBaseScene;

/// QtQuick wrapper for a sofa component (i.e baseObject / baseNode), allowing us to share a component in a QML context
class SOFA_SOFAQTQUICKGUI_API SofaComponent : public QObject
{
    Q_OBJECT

    friend class SofaData;

public:
    SofaComponent(SofaBaseScene* sofaScene, const sofa::core::objectmodel::Base* base);
    SofaComponent(const SofaComponent& sofaComponent);
    SofaComponent(const SofaComponent* sofaComponent);

public:
    Q_INVOKABLE QString getName() const;
    Q_INVOKABLE bool isNode() const;
    Q_INVOKABLE QStringList tags() const;
    Q_INVOKABLE QString className() const;
    Q_INVOKABLE QString namespaceName() const;
    Q_INVOKABLE QString templateName() const;
    Q_INVOKABLE QString description() const;
    Q_INVOKABLE QString providerName() const;
    Q_INVOKABLE QString getPathName() const;
    Q_INVOKABLE QObject* parent() ;

    Q_INVOKABLE bool isSame(SofaComponent* sofaComponent) const;
    Q_INVOKABLE sofaqtquick::bindings::SofaData* getComponentData(const QString& name) const;

    /// The following three are used to get extra information about where
    /// the component is implemented (the source location)
    /// and where it is instanciated (the scene file location)
    Q_INVOKABLE bool hasLocations() const ;
    Q_INVOKABLE QString getSourceLocation() const ;
    Q_INVOKABLE QString getCreationLocation() const ;

	Q_INVOKABLE QString output() const;
	Q_INVOKABLE QString warning() const;

	Q_INVOKABLE void reinit();

    Q_INVOKABLE sofa::qtquick::SofaBaseScene* sofaScene() const;

    static bool hasTag(sofa::core::objectmodel::Base* base, const QStringList& tags)
    {
        if(!base)
            return false;

        if(tags.isEmpty())
            return true;

        for(const QString& tag : tags)
            if(base->hasTag(sofa::core::objectmodel::Tag(tag.toStdString())))
                return true;

        return false;
    }


public slots:
	void clearOutput();
	void clearWarning();

public:
	/// \brief check if the sofa component still exists, if you need to access the component it is recommended to directly call SofaComponent::base() because it already check the object existence
	bool isValid() const;

    sofa::core::objectmodel::Base* base();
    const sofa::core::objectmodel::Base* base() const;

private:
    SofaBaseScene*                              mySofaScene;
    const sofa::core::objectmodel::Base*    myBase;

};


} /// namespace qtquick
} /// namespace sofa

