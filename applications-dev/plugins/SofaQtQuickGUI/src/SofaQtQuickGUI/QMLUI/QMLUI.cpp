#include <iostream>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>

#include "QMLUI.h"

#include <sofa/helper/logging/Messaging.h>
#include <sofa/core/objectmodel/DataFileName.h>

#include <SofaQtQuickGUI/Helper/QtStl/QtStl.h>
using sofaqtquick::bindings::SofaBaseObject;
using sofaqtquick::bindings::SofaBase;

namespace sofa::helper::logging
{
    inline bool notMuted(const sofaqtquick::qmlui::QmlUILoader*){ return true; }         \
    inline ComponentInfo::SPtr getComponentInfo(const sofaqtquick::qmlui::QmlUILoader* t)
    {
        std::stringstream ss;
        ss<<"QmlUILoader(" << t << ")";
        return ComponentInfo::SPtr(new ComponentInfo(ss.str())) ;
    }
}


namespace sofaqtquick::qmlui
{

QmlUILoader::QmlUILoader(QObject* parent)
{
}

void QmlUILoader::resetAndLoadAll(QList<QObject*> list)
{
    /// Remove the existing items.
    for(auto& item : m_loadedItems)
    {
        item->setParent(nullptr);
        item->setParentItem(nullptr);
    }

    m_loadedItems.clear();
    /// Re-add all of them.
    for(auto& o : list)
    {
        load(dynamic_cast<sofaqtquick::bindings::SofaBaseObject*>(o));
    }
}

void QmlUILoader::load(SofaBaseObject* canvas)
{
    std::cout << "loading canvas " << canvas->getName() << ". Status: " << canvas->self()->m_componentstate << std::endl;
    if (canvas->self()->m_componentstate == sofa::core::objectmodel::ComponentState::Loading ||
            canvas->self()->m_componentstate == sofa::core::objectmodel::ComponentState::Invalid)
        canvas->init();
    QQmlEngine* engine = qmlEngine(this);
    if(!engine)
    {
        msg_error() << "Unable to load component, missing engine." ;
        return;
    }

    engine->clearComponentCache();
    auto file = dynamic_cast<sofa::core::objectmodel::DataFileName*>(canvas->rawBase()->findData("filename"));
    QQmlComponent component(engine, QUrl(QString::fromStdString(file->getFullPath())));
    if(!component.isReady())
    {
        msg_error() << component.errorString();
        return;
    }

    QQuickItem *childItem = qobject_cast<QQuickItem*>(component.create());
    if(!childItem)
    {
        msg_error() << "Unable to instanciate component from: " << file->getFullPath() ;
        return;
    }

    childItem->setParentItem(this);
    childItem->setProperty("__componentPath", canvas->getPathName());
    m_loadedItems.push_back(childItem);
}


QmlUILoader::~QmlUILoader()
{
    /// Remove the existing items.
    for(auto& item : m_loadedItems)
    {
        item->setParent(nullptr);
        item->setParentItem(nullptr);
    }
}



}
