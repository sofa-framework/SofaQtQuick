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

void QmlUIManager::loadCanvasFrom(const QUrl& filename)
{
    std::cout << "LOAD CANVAS " << filename.toLocalFile() << std::endl;
    emit notifyLoadCanvasFrom(filename);
}

static QmlUIManager s_qmluimanager;
void QmlUIManagerSingleton::LoadCanvasFrom(const QUrl& filename)
{
    s_qmluimanager.loadCanvasFrom(filename);
}

QmlUIManager* QmlUIManagerSingleton::GetInstance()
{
    return &s_qmluimanager;
}

QmlUILoader::QmlUILoader(QObject* parent)
{
    connect(QmlUIManagerSingleton::GetInstance(),
            &QmlUIManager::notifyLoadCanvasFrom,
            this, &QmlUILoader::load);
    msg_error() << "Created." ;
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
    msg_error() << "Reload everything..." << list.size();
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
    childItem->setParentItem(this);
    m_loadedItems.push_back(childItem);
}


QmlUILoader::~QmlUILoader()
{
    msg_error() << "Destroyed" ;
    disconnect(QmlUIManagerSingleton::GetInstance(),
               &QmlUIManager::notifyLoadCanvasFrom,
               this, &QmlUILoader::load);
}



}
