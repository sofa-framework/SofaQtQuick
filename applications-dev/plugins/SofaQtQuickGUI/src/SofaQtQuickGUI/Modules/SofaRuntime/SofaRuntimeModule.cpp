#include <qqml.h>

#include <SofaQtQuickGUI/Modules/SofaRuntime/SofaRuntimeModule.h>
#include <SofaQtQuickGUI/QMLUI/QMLUI.h>
using sofaqtquick::qmlui::QmlUILoader;
using sofaqtquick::qmlui::QmlUIManager;
using sofaqtquick::qmlui::QmlUIManagerSingleton;

namespace sofaqtquick
{
const int versionMajor = 1;
const int versionMinor = 0;

// Following the doc on creating a singleton component
// we need to have function that return the singleton instance.
// see: http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType
static QObject* getQmlUIManagerSingleton(QQmlEngine *engine,
                                         QJSEngine *scriptEngine){
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return QmlUIManagerSingleton::GetInstance();
}

void registerTypesToQml()
{
    qmlRegisterType<QmlUILoader> ("SofaRuntime.QmlUILoader",
                                  versionMajor, versionMinor,
                                  "QmlUILoader");

    qmlRegisterSingletonType<QmlUIManager> ("SofaRuntime.QmlUIManagerSingleton",
                                            versionMajor, versionMinor,
                                            "QmlUIManagerSingleton",
                                            getQmlUIManagerSingleton);
}

void SofaRuntimeModule::RegisterTypes()
{
    static bool inited=false;
    if(!inited)
    {
        registerTypesToQml();
        inited=true;
    }
}

} /// namespace sofaqtquick
