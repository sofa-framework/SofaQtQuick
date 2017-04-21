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

#ifndef SOFAQTQUICKSOFA_H
#define SOFAQTQUICKSOFA_H

#include <sofa/helper/system/config.h>

#ifdef SOFA_BUILD_SOFAQTQUICKGUI
#define SOFA_SOFAQTQUICKGUI_API SOFA_EXPORT_DYNAMIC_LIBRARY
#else
#define SOFA_SOFAQTQUICKGUI_API SOFA_IMPORT_DYNAMIC_LIBRARY
#endif

#include <QQmlExtensionPlugin>

/// Forward declaration.
namespace sofa {
namespace qtquick {
namespace console {
    class Console;
}}}

/// \class A tool class to init the SofaQtQuickGUI plugin as a Qt plugin
class SOFA_SOFAQTQUICKGUI_API SofaQtQuickGUI : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "SofaQtQuickGUI")

public:
    explicit SofaQtQuickGUI(QObject *parent = 0);

    static sofa::qtquick::console::Console* getConsoleSingleton() ;
    static void setConsoleSingleton(sofa::qtquick::console::Console*) ;

private:
    void init();
    void registerTypes(const char *uri);

    static sofa::qtquick::console::Console* s_consolesingleton ;
};

#endif // SOFAQTQUICKSOFA_H
