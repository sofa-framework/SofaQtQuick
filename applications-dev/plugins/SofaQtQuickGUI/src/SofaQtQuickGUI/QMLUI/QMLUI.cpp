#include <iostream>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>

#include "QMLUI.h"

#include <sofa/helper/logging/Messaging.h>

#include <SofaQtQuickGUI/Helper/QtStl/QtStl.h>

namespace sofaqtquick::qmlui
{

QmlUILoader::QmlUILoader(QObject* parent)
{
    std::cout << "QMLLOADER CREATED " << parent << std::endl ;
}


void QmlUILoader::bindContext()
{
    QQmlEngine* engine = qmlEngine(this);
    if(!engine)
    {
        msg_error("QmlUILoader") << "Unable to load component, missing engine." ;
        return;
    }

    QQmlComponent component(engine, QUrl("qrc:/SofaBasics/DefaultQmlUICanvas.qml"));
    if(!component.isReady())
    {
        msg_error("QmlUILoader") << component.errorString();
        return;
    }

    QQuickItem *childItem = qobject_cast<QQuickItem*>(component.create());
    childItem->setParentItem(this);
}


}
