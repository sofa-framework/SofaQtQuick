#pragma once

#include <sofa/helper/system/config.h>

#ifdef SOFA_BUILD_QMLUI
#define SOFA_TARGET QmlUI
#define SOFA_QMLUI_API SOFA_EXPORT_DYNAMIC_LIBRARY
#else
#define SOFA_QMLUI_API SOFA_IMPORT_DYNAMIC_LIBRARY
#endif
