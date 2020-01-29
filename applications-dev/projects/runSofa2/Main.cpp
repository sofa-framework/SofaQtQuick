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
#include <sofa/helper/logging/MessageDispatcher.h>
using  sofa::helper::logging::MessageDispatcher;

#include <sofa/core/logging/PerComponentLoggingMessageHandler.h>
using  sofa::helper::logging::MainPerComponentLoggingMessageHandler;

#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <QtWebView/QtWebView>
#include <runSofa2/runSofa2.h>
#include <QQuickStyle>

#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::unique_lock, std::defer_lock

std::mutex mtx;           // mutex for critical section


void convertQMessagesToSofa(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    std::unique_lock<std::mutex> lck (mtx,std::defer_lock);
     // critical section (exclusive access to std::cout signaled by locking lck):
     lck.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    const char* file="undefined";
    if(context.file)
        file=context.file;

    const char* function="runSofa2";
    if(context.function)
        function=context.function;

    /// Promote the message to error if they contains "error" in their text message.
    if( localMsg.contains("Error") )
        type = QtCriticalMsg;

    if( localMsg.contains("qml:") && !strcmp(function, "runSofa2"))
        function = "qml";

    switch (type) {
    case QtDebugMsg:
        dmsg_info_withfile(function, file, context.line) << localMsg.constData();
        break;
    case QtInfoMsg:
        msg_info_withfile(function, file, context.line) << localMsg.constData();
        break;
    case QtWarningMsg:
        msg_warning_withfile(function, file, context.line) << localMsg.constData();
        break;
    case QtCriticalMsg:
        msg_error_withfile(function, file, context.line) << localMsg.constData();
        break;
    case QtFatalMsg:
        msg_error_withfile(function, file, context.line) << localMsg.constData();
        //abort();
    }
    lck.unlock();
}

int main(int argc, char **argv)
{
    /// Install the handler the Sofa message hook into the Qt messaging system.
    MessageDispatcher::addHandler(&MainPerComponentLoggingMessageHandler::getInstance()) ;


    /// IMPORTANT NOTE: this function MUST be call before QApplication creation in order to be able to load a SofaScene containing calls to OpenGL functions (e.g. containing OglModel)
    sofaqtquick::SofaBaseApplication::Initialization();

    QApplication app(argc, argv);
    QtWebView::initialize();
    QQmlApplicationEngine applicationEngine;
    QQuickStyle::setStyle("Imagine");
    /// application specific settings
    app.setOrganizationName("Sofa Consortium");
    app.setApplicationName("runSofa2");
    app.setApplicationVersion("v1.0");

    /// common settings for most sofaqtquick applications
    if(!sofaqtquick::SofaBaseApplication::DefaultMain(app, applicationEngine, "qrc:/qml/Main.qml"))
        return -1;

    return app.exec();
}

