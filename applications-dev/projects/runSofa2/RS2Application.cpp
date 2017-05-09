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
#include <QCommandLineParser>
#include <QPluginLoader>
#include <QQuickStyle>
#include <QSettings>
#include <QWindow>

#include <sofa/helper/system/console.h>
using sofa::helper::Console;

#include <sofa/helper/logging/MessageDispatcher.h>
using sofa::helper::logging::MessageDispatcher ;

#include <sofa/helper/logging/ClangMessageHandler.h>
using sofa::helper::logging::ClangMessageHandler ;

#include <sofa/helper/logging/ConsoleMessageHandler.h>
using sofa::helper::logging::ConsoleMessageHandler ;

#include <sofa/core/logging/PerComponentLoggingMessageHandler.h>
using  sofa::helper::logging::MainPerComponentLoggingMessageHandler ;


#include <sofa/core/logging/RichConsoleStyleMessageFormatter.h>
using  sofa::helper::logging::RichConsoleStyleMessageFormatter ;

#if 0
#include <iostream>
using std::string ;

#include <vector>
using std::vector;

#include <sofa/helper/BackTrace.h>
using sofa::helper::BackTrace ;

#include <sofa/helper/logging/ClangMessageHandler.h>
using sofa::helper::logging::ClangMessageHandler ;

#include <sofa/helper/logging/ConsoleMessageHandler.h>
using sofa::helper::logging::ConsoleMessageHandler ;

#include <sofa/core/logging/PerComponentLoggingMessageHandler.h>
using  sofa::helper::logging::MainPerComponentLoggingMessageHandler ;

#include <sofa/core/logging/RichConsoleStyleMessageFormatter.h>
using  sofa::helper::logging::RichConsoleStyleMessageFormatter ;

#include <sofa/helper/logging/MessageDispatcher.h>
using sofa::helper::logging::MessageDispatcher ;

#include <sofa/helper/logging/ClangMessageHandler.h>
using sofa::helper::logging::ClangMessageHandler ;

#include <sofa/helper/logging/ConsoleMessageHandler.h>
using sofa::helper::logging::ConsoleMessageHandler ;

#include <sofa/core/logging/PerComponentLoggingMessageHandler.h>
using  sofa::helper::logging::MainPerComponentLoggingMessageHandler ;
#endif

#include "SofaQtQuickGUI/SofaScene.h"
using sofa::qtquick::SofaScene ;

#include <sofa/helper/AdvancedTimer.h>
using sofa::helper::AdvancedTimer ;

#include <sofa/helper/system/FileRepository.h>
using sofa::helper::system::DataRepository ;

#include "SofaQtQuickGUI/SofaApplication.h"
using sofa::qtquick::MainApplication ;

#include "RS2Application.h"

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <signal.h>
#endif

namespace sofa
{
namespace rs
{
namespace _rs2application_
{

RS2Application::RS2Application(QObject* parent) : DefaultApplication(parent)
{
    std::cout << "CREATING THE RS2 APPLICATION" << std::endl ;
}

RS2Application::~RS2Application() {}

void RS2Application::prepareCommandLine(QCommandLineParser& parser)
{
    QCommandLineOption sceneOption({"s", "scene"}, "Load with this scene", "file");
    QCommandLineOption animateOption({"a", "animate"}, "Start in animate mode");
    QCommandLineOption fullscreenOption({"f", "fullscreen"}, "Fullscreen mode");
    QCommandLineOption widthOption("width", "Window width", "pixels");
    QCommandLineOption heightOption("height", "Window height", "pixels");
    QCommandLineOption logTimeOption("log", "Log time during simulation");
    QCommandLineOption colorOption({"z", "colors"}, "Use colors on stdout and stderr (yes, no, auto)");
    QCommandLineOption messageHandlerOption({"f", "formatting"}, "Select the message formatting to use (sofa-old, sofa-new, clang)");
    QCommandLineOption loadPluginOption({"l", "load"}, "Load given plugins" );

    parser.addOption(sceneOption);
    parser.addOption(animateOption);
    parser.addOption(fullscreenOption);
    parser.addOption(widthOption);
    parser.addOption(heightOption);
    parser.addOption(logTimeOption);
    parser.addOption(colorOption);
    parser.addOption(messageHandlerOption);
    parser.addOption(loadPluginOption);

    parser.addVersionOption();
    parser.addHelpOption();
}

bool  RS2Application::mainInitialization(QApplication& app,
                                         QQmlApplicationEngine &applicationEngine,
                                         const QString& mainScript)
{
    // application style
    QQuickStyle::setStyle("Material");

    // TODO: this command disable the multithreaded render loop, currently we need this because our implementation of the sofa interface is not thread-safe
    qputenv("QSG_RENDER_LOOP", "basic");

    if(!app.testAttribute(Qt::AA_ShareOpenGLContexts))
        qCritical() << "CRITICAL: RS2Applicationinitialization() must be called before QApplication instanciation";

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    // quit on Ctrl-C
    signal(SIGINT, [](int) {MainApplication::Destruction();});
#endif

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    app.addLibraryPath(QCoreApplication::applicationDirPath() + "/../lib/");

    QSettings::setPath(QSettings::Format::IniFormat, QSettings::Scope::UserScope, QCoreApplication::applicationDirPath() + "/config/");

    // initialise paths
    useDefaultSofaPath();

    // add application data path
    sofa::helper::system::DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/" + app.applicationName() + "Data").toStdString());
    sofa::helper::system::DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/../data").toStdString());
    sofa::helper::system::DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/../" + app.applicationName() + "Data").toStdString());

    // plugin initialization
    QString pluginName("SofaQtQuickGUI");
#ifndef NDEBUG
    pluginName += "_d";
#endif
    QPluginLoader pluginLoader(pluginName);

    // first call to instance() initialize the plugin
    if(0 == pluginLoader.instance()) {
        qCritical() << pluginLoader.errorString();
        qCritical() << "ERROR: SofaQtQuickGUI plugin has not been found "<<pluginName;
        return false;
    }

    // apply the app settings or use the default.ini settings if it is the
    // first time the user launch the application or use the app.backup.ini in case of application crash
    applySettings();

    // launch the main script
    applicationEngine.addImportPath("qrc:/");
    applicationEngine.addImportPath(QCoreApplication::applicationDirPath() + "/../lib/qml/");
    applicationEngine.load(QUrl(mainScript));


    // compute command line arguments
    QCommandLineParser parser;

    prepareCommandLine(parser);
    parser.parse(app.arguments());

    if(parser.isSet("version"))
    {
        parser.showVersion();
        return true;
    }

    if(parser.isSet("help"))
    {
        parser.showHelp();
        return true;
    }

    if(parser.isSet("log")){
        sofa::helper::AdvancedTimer::setEnabled("Animate", true);
    }

    // alphabetical order on short name
    //TODO(dmarchal 2017-05-05/delete in 1 year): These probably should in a separated application 'runSofaViewer'.
    //.option(&nbMSSASamples, 'm', "msaa", "number of samples for MSAA (Multi Sampling Anti Aliasing ; value < 2 means disabled")
    //.option(&startAnim,'a',"start","start the animation loop")
    //.option(&nbIterations,'n',"nb_iterations","(only batch) Number of iterations of the simulation")
    //TODO(dmarchal 2017-05-05/delete in 1 year): These seems specfic to a batch mode 'runSofaBatch'.
    //.option(&temporaryFile,'t',"temporary","the loaded scene won't appear in history of opened files")
    //.option(&testMode,'x',"test","select test mode with xml output after N iteration")
    //.option(&verif,'v',"verification","load verification data for the scene")
    //TODO(dmarchal 2017-05-05/delete in 1 year): These seems the one to support for 17.06 release of runSofa
    //.option(&simulationType,'s',"simu","select the type of simulation (bgl, dag, tree, smp)")
    //.option(&printFactory,'p',"factory","print factory logs")
    //.option(&computationTimeSampling,'c',"computationTimeSampling","Frequency of display of the computation time statistics, in number of animation steps. 0 means never.")

    QString colorsStatus = "auto";
    if(parser.isSet("colors"))
        colorsStatus = parser.value("colors");


    if (colorsStatus == "auto")
        Console::setColorsStatus(Console::ColorsAuto);
    else if (colorsStatus == "yes")
        Console::setColorsStatus(Console::ColorsEnabled);
    else if (colorsStatus == "no")
        Console::setColorsStatus(Console::ColorsDisabled);


    QString messageHandler = "sofa-old";
    if(parser.isSet("formatting"))
        messageHandler = parser.value("formatting");

    /// This activate the logging of the message in each component so this can be displayed in
    /// a specific way in the GUI. I personally don't think this is the proper way to do that but
    /// this is the way it was done in previous version of sofa so we will keep that for at least
    /// one or two year (so we may have solve that around 2019).
    /// TODO(dmarchal 2017-05-05/delete in 2 year): try a better approach.
    MessageDispatcher::clearHandlers() ;
    MessageDispatcher::addHandler(&MainPerComponentLoggingMessageHandler::getInstance()) ;

    if (messageHandler == "sofa-old")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ConsoleMessageHandler() ) ;
    }
    else if (messageHandler == "sofa-new")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ConsoleMessageHandler(new RichConsoleStyleMessageFormatter()) ) ;
    }
    else if (messageHandler == "clang")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ClangMessageHandler() ) ;
    }
    else
    {
        msg_warning("RS2Application") << "Invalid formatting ‘" << messageHandler.toStdString() << "‘";
    }

    /// apply command line arguments ??? Il y a plusieurs ROOT ?
    QList<QObject*> objects = applicationEngine.rootObjects();
    foreach(QObject* object, objects)
    {
        QWindow* window = qobject_cast<QWindow*>(object);
        if(!window)
            continue;

        if(parser.isSet("animate") || parser.isSet("scene"))
        {
            SofaScene* sofaScene = object->findChild<SofaScene*>();
            if(parser.isSet("scene"))
            {
                sofaScene->setSource(parser.value("scene"));
            }
            if(parser.isSet("animate"))
            {
                sofaScene->setDefaultAnimate(true);
                if(sofaScene->isReady())
                    sofaScene->setAnimate(true);
            }
        }

        if(parser.isSet("width"))
            window->setWidth(parser.value("width").toInt());

        if(parser.isSet("height"))
            window->setHeight(parser.value("height").toInt());

        if(parser.isSet("fullscreen"))
            window->setVisibility(QWindow::FullScreen);
    }

    return true;
}

#if 0
vector<string> files;
string fileName ;
string colorsStatus = "auto";
string messageHandler = "auto";
bool enableInteraction = false ;


/// Make something with the command lines variables.


///TODO(dmarchal 2017-05-05): Use smart pointer there to avoid memory leaks !!

/// This activate the logging of the message in each component so this can be displayed in
/// a specific way in the GUI. I personally don't think this is the proper way to do that but
/// this is the way it was done in previous version of sofa so we will keep that for at least
/// one or two year (so we may have solve that around 2019).
/// TODO(dmarchal 2017-05-05/delete in 2 year): try a better approach.
MessageDispatcher::addHandler(&MainPerComponentLoggingMessageHandler::getInstance()) ;

if(enableInteraction)
{
    msg_warning("runSofa2") << "you activated the interactive mode. This is currently an experimental feature "
                               "that may change or be removed in the future. " ;
}

if (files.size()>1)
{
    msg_warning("runSofa2") << "Expecting only one scene file in the commande line. But there seem to have more. "
                               "The other are ignored" ;
}

if (!files.empty())
fileName = files[0];

if (fileName.empty())
{
    if (loadRecent) // try to reload the latest scene
    {
        string scenes = BaseGUI::getConfigDirectoryPath() + "/runSofa.ini";
        std::ifstream mrulist(scenes.c_str());
        std::getline(mrulist,fileName);
        mrulist.close();
    }
    else
        fileName = "Demos/caduceus.scn";

    fileName = DataRepository.getFile(fileName);
}
#endif


} /// namespace _rs2application_
} /// namespace rs
} /// namespace sofa
