/*
Copyright 2017, CNRS

This file is part of runSofa2.

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.

Contributors:
    - damien.marchal@univ-lille1.fr.
*/

#include <sofa/helper/BackTrace.h>
using sofa::helper::BackTrace ;

#include "SofaQtQuickGUI/SofaApplication.h"
using sofa::qtquick::MainApplication ;

#include "RS2Application.h"
using sofa::rs::RS2Application ;


#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <signal.h>
#endif

int main(int argc, char **argv)
{
    MainApplication::setApplicationSingleton(new RS2Application()) ;

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    /// quit on Ctrl-C
    signal(SIGINT, [](int) {MainApplication::Destruction();});
#endif

    /// IMPORTANT NOTE: this function MUST be call before QApplication creation in order to be able to
    /// load a SofaScene containing calls to OpenGL functions (e.g. containing OglModel)
    MainApplication::Initialization();

    /// application specific settings
    QApplication app(argc, argv);
    app.setOrganizationName("Sofa Consortium");
    app.setApplicationName("runSofa2");
    app.setApplicationVersion("v1.0");

    QQmlApplicationEngine applicationEngine;

    if(!MainApplication::MainInitialization(app, applicationEngine, "qrc:/qml/Main.qml"))
        exit(0);

    /// Let's start runSofa2.
    return app.exec();
}
