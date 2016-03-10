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

#include <GL/glew.h>
#include "SofaComponent.h"
#include "SofaScene.h"

#include <sofa/core/ObjectFactory.h>

#include <qqml.h>
#include <QQmlEngine>

namespace sofa
{

namespace qtquick
{

using namespace sofa::core::objectmodel;

SofaComponent::SofaComponent(SofaScene* sofaScene, const sofa::core::objectmodel::Base* base) : QObject(),
    mySofaScene(sofaScene),
    myBase(base)
{

}

SofaComponent::SofaComponent(const SofaComponent& sofaComponent) : QObject(),
    mySofaScene(sofaComponent.sofaScene()),
    myBase(sofaComponent.base())
{

}

QString SofaComponent::name() const
{
    const Base* base = SofaComponent::base();
    if(base)
        return QString::fromStdString(base->getName());

    return QString("Invalid SofaComponent");
}

QString SofaComponent::className() const
{
    const Base* base = SofaComponent::base();
    if(base)
        return QString::fromStdString(base->getClassName());

    return "";
}

QString SofaComponent::namespaceName() const
{
    const Base* base = SofaComponent::base();
    if(base)
        return QString::fromStdString(core::objectmodel::BaseClass::decodeNamespaceName(typeid(*base)));

    return "";
}

QString SofaComponent::templateName() const
{
    const Base* base = SofaComponent::base();
    if(base)
        return QString::fromStdString(base->getTemplateName());

    return "";
}

QString SofaComponent::description() const
{
    const Base* base = SofaComponent::base();
    if(base)
    {
        core::ObjectFactory::ClassEntry entry = core::ObjectFactory::getInstance()->getEntry(base->getClassName());
        if(!entry.creatorMap.empty())
            if(!entry.description.empty() && std::string("TODO") != entry.description)
                return QString::fromStdString(entry.description);
    }

    return "";
}

QString SofaComponent::providerName() const
{
    const Base* base = SofaComponent::base();
    if(base)
    {
        core::ObjectFactory::ClassEntry entry = core::ObjectFactory::getInstance()->getEntry(base->getClassName());
        if(!entry.creatorMap.empty())
        {
            core::ObjectFactory::CreatorMap::iterator it = entry.creatorMap.find(base->getTemplateName());
            if(entry.creatorMap.end() != it && *it->second->getTarget())
                return QString::fromStdString(it->second->getTarget());
        }
    }

    return "";
}

bool SofaComponent::isSame(SofaComponent* sofaComponent) const
{
    if(!sofaComponent)
        return false;

    // same wrapper => same component
    if(this == sofaComponent)
        return true;

    // same base object => same component
    if(base() == sofaComponent->base())
        return true;

    return false;
}

SofaData* SofaComponent::getComponentData(const QString& name) const
{
    sofa::core::objectmodel::BaseData* data = myBase->findData(name.toStdString());
    if(!data)
        return 0;

    return new SofaData(this, data);
}

QString SofaComponent::output() const
{
	const Base* base = SofaComponent::base();
	if(base)
		return QString::fromStdString(base->getOutputs());

	return QString();
}

QString SofaComponent::warning() const
{
	const Base* base = SofaComponent::base();
	if(base)
		return QString::fromStdString(base->getWarnings());

	return QString();
}

void SofaComponent::clearOutput()
{
	Base* base = SofaComponent::base();
	if(base)
	{
		base->clearOutputs();
	}
}

void SofaComponent::clearWarning()
{
	Base* base = SofaComponent::base();
	if(base)
	{
		base->clearWarnings();
	}
}

void SofaComponent::reinit()
{
	Base* base = SofaComponent::base();
	if(!base)
		return;

	BaseObject* baseObject = dynamic_cast<BaseObject*>(base);
	if(!baseObject)
		return;

	baseObject->reinit();
}

SofaScene* SofaComponent::sofaScene() const
{
    return mySofaScene;
}

bool SofaComponent::isValid() const
{
	return base();
}

Base* SofaComponent::base()
{
    return const_cast<Base*>(static_cast<const SofaComponent*>(this)->base());
}

const Base* SofaComponent::base() const
{
    const Base* base = nullptr;

    // check object existence
    if(mySofaScene && myBase)
        if(mySofaScene->componentExists(myBase))
            base = myBase;

    return base;
}

}

}
