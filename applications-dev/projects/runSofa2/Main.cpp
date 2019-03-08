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

#include <sofa/helper/logging/Messaging.h>
#include <SofaQtQuickGUI/SofaApplication.h>
#include <QtWebView/QtWebView>
#include <runSofa2/runSofa2.h>
#include <QQuickStyle>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        dmsg_info_withfile("runSofa2", context.file, context.line) << localMsg.constData();
        break;
    case QtInfoMsg:
        msg_info_withfile("runSofa2", context.file, context.line) << localMsg.constData();
        break;
    case QtWarningMsg:
        msg_warning_withfile("runSofa2", context.file, context.line) << localMsg.constData();
        break;
    case QtCriticalMsg:
        msg_error_withfile("runSofa2", context.file, context.line) << localMsg.constData();
        break;
    case QtFatalMsg:
        msg_error_withfile("runSofa2", context.file, context.line) << localMsg.constData();
        //abort();
    }
}

int main(int argc, char **argv)
{
    qInstallMessageHandler(myMessageOutput); // Install the handler

    // IMPORTANT NOTE: this function MUST be call before QApplication creation in order to be able to load a SofaScene containing calls to OpenGL functions (e.g. containing OglModel)
    sofa::qtquick::SofaApplication::Initialization();

    QApplication app(argc, argv);
    QtWebView::initialize();
    QQmlApplicationEngine applicationEngine;
    QQuickStyle::setStyle("Imagine");

    // application specific settings
    app.setOrganizationName("Sofa Consortium");
    app.setApplicationName("runSofa2");
    app.setApplicationVersion("v1.0");


    // common settings for most sofaqtquick applications
    if(!sofa::qtquick::SofaApplication::DefaultMain(app, applicationEngine, "qrc:/qml/Main.qml"))
        return -1;

    return app.exec();
}

