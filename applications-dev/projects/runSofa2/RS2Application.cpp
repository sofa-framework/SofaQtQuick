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
#include <string>
using std::string;

#include <QCommandLineParser>
#include <QPluginLoader>
#include <QQuickStyle>
#include <QSettings>
#include <QWindow>

#include <sofa/helper/BackTrace.h>
using sofa::helper::BackTrace;

#include <sofa/helper/system/console.h>
using sofa::helper::Console;

#include "sofa/helper/Utils.h"
using sofa::helper::Utils ;

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

#include "SofaQtQuickGUI/SofaScene.h"
using sofa::qtquick::SofaScene ;

#include <sofa/helper/AdvancedTimer.h>
using sofa::helper::AdvancedTimer ;

#include <sofa/helper/system/FileRepository.h>
using sofa::helper::system::DataRepository ;
using sofa::helper::system::PluginRepository ;

#include <sofa/helper/system/PluginManager.h>
using sofa::helper::system::PluginManager ;

#include "SofaQtQuickGUI/SofaApplication.h"
using sofa::qtquick::MainApplication ;


#include "RS2Application.h"

namespace sofa
{
namespace rs
{
namespace _rs2application_
{

RS2Application::RS2Application(QObject* parent) : DefaultApplication(parent)
{

}

RS2Application::~RS2Application() {}

void RS2Application::setMessageType(const QString& handleType)
{
    /// This activate the logging of the message in each component so this can be displayed in
    /// a specific way in the GUI. I personally don't think this is the proper way to do that but
    /// this is the way it was done in previous version of sofa so we will keep that for at least
    /// one or two year (so we may have solve that around 2019).
    /// TODO(dmarchal 2017-05-05/delete in 2 year): try a better approach.
    MessageDispatcher::clearHandlers() ;
    MessageDispatcher::addHandler(&MainPerComponentLoggingMessageHandler::getInstance()) ;

    if (handleType == "sofa-old")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ConsoleMessageHandler() ) ;
    }
    else if (handleType == "sofa-new")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ConsoleMessageHandler(new RichConsoleStyleMessageFormatter()) ) ;
    }
    else if (handleType == "clang")
    {
        MessageDispatcher::clearHandlers() ;
        MessageDispatcher::addHandler( new ClangMessageHandler() ) ;
    }
    else
    {
        msg_warning("RS2Application") << "Invalid message formatting ‘" << handleType.toStdString() << "‘";
    }
}

void RS2Application::setConsoleMode(const QString& colorMode)
{
    if (colorMode == "auto")
        Console::setColorsStatus(Console::ColorsAuto);
    else if (colorMode == "yes")
        Console::setColorsStatus(Console::ColorsEnabled);
    else if (colorMode == "no")
        Console::setColorsStatus(Console::ColorsDisabled);
}

void RS2Application::loadSofaPlugins(const QStringList& plugins)
{
    /// Add the plugin directory to PluginRepository
#ifdef WIN32
    const string pluginDir = Utils::getExecutableDirectory();
#else
    const string pluginDir = Utils::getSofaPathPrefix() + "/lib";
#endif
    PluginRepository.addFirstPath(pluginDir);

    /// Loads the plugins provided by users.
    for (auto& plugin : plugins)
        PluginManager::getInstance().loadPlugin(plugin.toStdString());

    /// no to get an error on the console if SofaPython does not exist
    std::ostringstream no_error_message;
    PluginManager::getInstance().loadPlugin("SofaPython", &no_error_message);

    PluginManager::getInstance().init();
}

void RS2Application::processCommandLineOptions(const QStringList& arguments)
{
    // compute command line arguments
    QCommandLineParser parser;
    QCommandLineOption sceneOption({"s", "scene"}, "Load with this scene", "file");
    QCommandLineOption animateOption({"a", "animate"}, "Start in animate mode");
    QCommandLineOption fullscreenOption({"f", "fullscreen"}, "Fullscreen mode");
    QCommandLineOption widthOption("width", "Window width", "width");
    QCommandLineOption heightOption("height", "Window height", "height");
    QCommandLineOption logTimeOption({"l", "log"}, "Log time during simulation");

    QCommandLineOption colorOption({"z", "colors"},
                                   "Use colors on the console printing (yes, no, auto)",
                                   "color", "auto");

    QCommandLineOption messageHandlerOption({"m", "message"},
                                            "Select the message formatting to use (sofa-old, sofa-new, clang)",
                                            "syle", "sofa-old");

    QCommandLineOption loadPluginOption({"p", "preload"}, "Load given plugins", "plugin,...");

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

    parser.parse(arguments);

    if(parser.isSet("version"))
    {
        parser.showVersion();
        return; /// Not needed because showVersion already exit the application
    }

    if(parser.isSet("help"))
    {
        parser.showHelp();
        return; /// Not needed because showVersion already exit the application
    }

    if(parser.isSet("log"))
    {
        sofa::helper::AdvancedTimer::setEnabled("Animate", true);
    }

    setConsoleMode( parser.value("colors") ) ;

    setMessageType( parser.value("message") ) ;

    if(parser.isSet("width"))
        m_defaultWidth = parser.value("width").toInt();

    if(parser.isSet("height"))
        m_defaultHeight = parser.value("height").toInt();

    if(parser.isSet("fullscreen"))
        m_defaultVisibility = QWindow::FullScreen ;

    if(parser.isSet("scene"))
        m_defaultScene = parser.value("scene") ;

    if(parser.isSet("animate"))
        m_animateOnLoad = true ;

    /// We then open the provided plugins.
    if(parser.isSet("preload"))
    {
        loadSofaPlugins(parser.value("preload").split(","));
    }
}

bool RS2Application::getAnimateOnLoad()
{
    return m_animateOnLoad ;
}

int RS2Application::getDefaultWidth()
{
    return m_defaultWidth ;
}

int RS2Application::getDefaultHeight()
{
    return m_defaultHeight ;
}

QString RS2Application::getDefaultScene()
{
    return m_defaultScene ;
}

QWindow::Visibility RS2Application::getDefaultVisibility()
{
    return m_defaultVisibility ;
}

/// Following the doc on creating a singleton component
/// we need to have function that return the singleton instance.
/// see: http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType
static QObject* getRS2Instance(QQmlEngine *engine,
                               QJSEngine *scriptEngine){
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return MainApplication::getApplicationSingleton() ;
}

bool  RS2Application::mainInitialization(QApplication& app,
                                         QQmlApplicationEngine &applicationEngine,
                                         const QString& mainScript)
{
    /// application style
    QQuickStyle::setStyle("Material");

    //TODO: this command disable the multithreaded render loop, currently we need this because our
    //implementation of the sofa interface is not thread-safe
    //TODO(dmarchal 2017-05-09): when is this suppose to be done, who is suppose to do it and please provides a deadline
    qputenv("QSG_RENDER_LOOP", "basic");

    if(!app.testAttribute(Qt::AA_ShareOpenGLContexts))
        qCritical() << "RS2Applicationinitialization() must be called before QApplication instanciation";

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    app.addLibraryPath(QCoreApplication::applicationDirPath() + "/../lib/");

    QSettings::setPath(QSettings::Format::IniFormat, QSettings::Scope::UserScope, QCoreApplication::applicationDirPath() + "/config/");

    useDefaultSofaPath();

    /// add application data path
    DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/" + app.applicationName() + "Data").toStdString());
    DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/../data").toStdString());
    DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/../" + app.applicationName() + "Data").toStdString());

    // plugin initialization
    QString pluginName("SofaQtQuickGUI");
#ifndef NDEBUG
    pluginName += "_d";
#endif
    QPluginLoader pluginLoader(pluginName);

    /// first call to instance() initialize the plugin
    if(0 == pluginLoader.instance()) {
        qCritical() << pluginLoader.errorString();
        qCritical() << "SofaQtQuickGUI plugin has not been found "<<pluginName;
        return false;
    }

    /// registers the C++ type in the QML system with the name "Console",
    qmlRegisterSingletonType<RS2Application>("RS2Application",                    /// char* uri
                                             1, 0,                                /// int majorVersion
                                             "RS2Application",                    /// exported Name.
                                             getRS2Instance);

    /// apply the app settings or use the default.ini settings if it is the
    /// first time the user launch the application or use the app.backup.ini in case of application crash
    applySettings();

    processCommandLineOptions(app.arguments()) ;

    /// launch the main script which instantiate the SofaApplication.qml part
    applicationEngine.addImportPath("qrc:/");
    applicationEngine.addImportPath(QCoreApplication::applicationDirPath() + "/../lib/qml/");
    applicationEngine.load(QUrl(mainScript));

    return true;
}

} /// namespace _rs2application_
} /// namespace rs
} /// namespace sofa
