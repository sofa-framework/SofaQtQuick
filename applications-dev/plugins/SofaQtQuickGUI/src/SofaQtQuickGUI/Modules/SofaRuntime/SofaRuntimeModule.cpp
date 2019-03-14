#include <qqml.h>

#include <SofaQtQuickGUI/Modules/SofaRuntime/SofaRuntimeModule.h>
#include <SofaQtQuickGUI/QMLUI/QMLUI.h>
using sofaqtquick::qmlui::QmlUILoader;

namespace sofaqtquick
{
const int versionMajor = 1;
const int versionMinor = 0;

void registerTypesToQml()
{
    qmlRegisterType<QmlUILoader> ("SofaRuntime.QmlUILoader",
                                  versionMajor, versionMinor,
                                  "QmlUILoader");
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
