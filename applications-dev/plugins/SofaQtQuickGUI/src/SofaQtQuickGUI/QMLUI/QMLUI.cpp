#include <iostream>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>

#include "QMLUI.h"

#include <sofa/helper/logging/Messaging.h>

#include <SofaQtQuickGUI/Helper/QtStl/QtStl.h>

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

void QmlUILoader::resetAndLoadAll(const QUrlList& list)
{
    /// Remove the existing items.
    for(auto& item : m_loadedItems)
    {
        item->setParent(nullptr);
        item->setParentItem(nullptr);
    }

    /// Re-add all of them.
    for(auto& url : list)
        load(url);
}

void QmlUILoader::load(const QUrl& filename)
{
    QQmlEngine* engine = qmlEngine(this);
    if(!engine)
    {
        msg_error() << "Unable to load component, missing engine." ;
        return;
    }

    QQmlComponent component(engine, QUrl(filename));
    if(!component.isReady())
    {
        msg_error() << component.errorString();
        return;
    }

    QQuickItem *childItem = qobject_cast<QQuickItem*>(component.create());
    if(!childItem)
    {
        msg_error() << "Unable to instanciate component from: " << filename.toString() ;
        return;
    }

    childItem->setParentItem(this);
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
