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

#include "ImageQtQuickGUI.h"

#include <QApplication>

#include "ImagePlaneView.h"
#include "ImagePlaneModel.h"

using namespace sofa::qtquick;

const int versionMajor = 1;
const int versionMinor = 0;

static void initResources()
{
    Q_INIT_RESOURCE(qml);
}

namespace sofa
{

namespace component
{

extern "C" {
    SOFA_IMAGE_QTQUICKGUI_API void initExternalModule();
    SOFA_IMAGE_QTQUICKGUI_API const char* getModuleName();
    SOFA_IMAGE_QTQUICKGUI_API const char* getModuleVersion();
    SOFA_IMAGE_QTQUICKGUI_API const char* getModuleLicense();
    SOFA_IMAGE_QTQUICKGUI_API const char* getModuleDescription();
    SOFA_IMAGE_QTQUICKGUI_API const char* getModuleComponentList();
}

void initExternalModule()
{
    static bool first = true;
    if (first)
    {
        first = false;

        initResources();

        qmlRegisterType<ImagePlaneModel>    ("ImagePlaneModel", versionMajor, versionMinor, "ImagePlaneModel");
        qmlRegisterType<ImagePlaneView>     ("ImagePlaneView",  versionMajor, versionMinor, "ImagePlaneView" );
    }
}

const char* getModuleName()
{
    return "Image Plugin - QtQuick GUI";
}

const char* getModuleVersion()
{
    return "0.1";
}

const char* getModuleLicense()
{
    return "LGPL";
}

const char* getModuleDescription()
{
    return "Image QtQuick GUI";
}

const char* getModuleComponentList()
{
    return "";
}

} // namespace component

} // namespace sofa
