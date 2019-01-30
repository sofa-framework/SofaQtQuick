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
#include <qqml.h>
#include <QQmlEngine>

#include <GL/glew.h>

#include "SofaComponent.h"
#include "SofaScene.h"

#include <sofa/core/ObjectFactory.h>

#include "sofa/helper/logging/Message.h"
using sofa::helper::logging::Message ;


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

SofaComponent::SofaComponent(const SofaComponent *sofaComponent) : QObject(),
    mySofaScene(sofaComponent->sofaScene()),
    myBase(sofaComponent->base())
{

}

QString SofaComponent::name() const
{
    const Base* base = SofaComponent::base();
    if(base)
        return QString::fromStdString(base->getName());

    return QString("Invalid SofaComponent");
}

QStringList SofaComponent::tags() const
{
    const Base* base = SofaComponent::base();
    QStringList tags;
    if(base)
        for (Tag const& t : base->getTags())
            tags << QString::fromStdString(std::string(t));
    return tags;
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

QString SofaComponent::getPathName() const
{
    const Base* base = SofaComponent::base();
    if(base) {
        const BaseObject* baseObject = dynamic_cast<const BaseObject*>(base);
        if (baseObject)
            return QString::fromStdString(baseObject->getPathName());

        const BaseNode* baseNode = dynamic_cast<const BaseNode*>(base);
        if (baseNode)
            return QString::fromStdString(baseNode->getPathName());
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
        return QString::fromStdString(base->getLoggedMessagesAsString({Message::Info, Message::Advice}));

    return QString();
}

QString SofaComponent::warning() const
{
    const Base* base = SofaComponent::base();
    if(base)
        return QString::fromStdString(base->getLoggedMessagesAsString({Message::Deprecated,
                                                                       Message::Warning,
                                                                       Message::Error,
                                                                       Message::Fatal
                                                                      }));

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
    if(mySofaScene && myBase){
        if(mySofaScene->componentExists(myBase))
            base = myBase;
    }
    return base;
}

QObject* SofaComponent::parent()
{
    BaseNode* base = dynamic_cast<BaseNode*>(SofaComponent::base());
    std::cout << "ICI" << (long long)base << std::endl ;
    if(base && base->getParents().size())
        return new SofaComponent(mySofaScene, base->getParents()[0]);

    return nullptr;
}

bool SofaComponent::hasLocations() const
{
    const Base* base = SofaComponent::base();
    if(base)
    {
        return base->findData("Defined in") != nullptr ;
    }
    return false;
}

QString SofaComponent::getSourceLocation() const
{
    const Base* base = SofaComponent::base();
    if(base)
    {
        BaseData* data = base->findData("Defined in") ;
        if(data)
            return QString::fromStdString(data->getValueString());
    }
    return "('',0)";
}

QString SofaComponent::getCreationLocation() const
{
    const Base* base = SofaComponent::base();
    if(base)
    {
        BaseData* data = base->findData("Instantiated in") ;
        if(data)
            return QString::fromStdString(data->getValueString());
    }
    return "('',0)";
}


}

}
