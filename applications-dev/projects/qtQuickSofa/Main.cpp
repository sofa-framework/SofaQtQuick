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

#include <sofa/helper/BackTrace.h>
using sofa::helper::BackTrace ;

#include <SofaQtQuickGUI/SofaApplication.h>
using sofa::qtquick::MainApplication ;

#include <SofaQtQuickGUI/DefaultApplication.h>
using sofa::qtquick::DefaultApplication ;

int main(int argc, char **argv)
{
    BackTrace::autodump();

    MainApplication::setApplicationSingleton(new DefaultApplication());

    // IMPORTANT NOTE: this function MUST be call before QApplication creation in order to be able to load a SofaScene containing calls to OpenGL functions (e.g. containing OglModel)
    MainApplication::Initialization();

    QApplication app(argc, argv);
    QQmlApplicationEngine applicationEngine;

    // application specific settings
    app.setOrganizationName("Sofa");
    app.setApplicationName("qtQuickSofa");
    app.setApplicationVersion("v1.0");

    // common settings for most sofaqtquick applications
    if(!sofa::qtquick::MainApplication::MainInitialization(app, applicationEngine, "qrc:/qml/Main.qml"))
        return -1;

    return app.exec();
}
