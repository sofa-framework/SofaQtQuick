/*
Copyright 2015, Anatoscope
Copyright 2017, CNRS

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

Contributors:
    - anatoscope
    - damien.marchal@univ-lille1.fr
*/

#include "DefaultApplication.h"
#include "SofaScene.h"
#include "ProcessState.h"

#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/Utils.h>
#include <sofa/helper/BackTrace.h>
#include <sofa/helper/system/console.h>
#include <sofa/helper/logging/Messaging.h>
#include <sofa/helper/logging/ConsoleMessageHandler.h>
#include <sofa/helper/AdvancedTimer.h>
#include <SofaPython/PythonEnvironment.h>

#include <QQuickStyle>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlContext>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QPluginLoader>
#include <QSettings>
#include <QScreen>
#include <QDir>
#include <QStack>
#include <QDebug>
#include <QCommandLineParser>
#include <QWindow>
#include <QDesktopServices>
#include <QClipboard>
#include <QQuickRenderControl>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QQmlComponent>
#include <QQuickItem>
#include <QOpenGLFramebufferObject>
#include <QRunnable>
#include <QTimer>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <signal.h>
#endif

namespace sofa
{

namespace qtquick
{

using namespace sofa::helper;
using namespace sofa::simulation;

DefaultApplication::DefaultApplication(QObject *parent) : SofaApplication(parent)
{
}

DefaultApplication::~DefaultApplication()
{
}

void DefaultApplication::setOpenGLDebugContext()
{
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);
}

void DefaultApplication::useOpenGLDebugLogger()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if(0 == ctx) {
        msg_error("SofaQtQuickGUI") << "DefaultApplication::eDebugLogger has been called without a valid opengl context made current";
        return;
    }

    msg_info("SofaQtQuickGUI") << "OpenGL Context" << (QString::number(ctx->format().majorVersion()) + "." + QString::number(ctx->format().minorVersion())).toLatin1().constData();
    msg_info("SofaQtQuickGUI") << "Graphics Card Vendor:" << (char*) glGetString(GL_VENDOR);
    msg_info("SofaQtQuickGUI") << "Graphics Card Model:" << (char*) glGetString(GL_RENDERER);
    msg_info("SofaQtQuickGUI") << "Graphics Card Drivers:" << (char*) glGetString(GL_VERSION);

    if(ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
    {
        QOpenGLDebugLogger* openglDebugLogger = new QOpenGLDebugLogger();
        if(!openglDebugLogger->initialize())
            msg_error("SofaQtQuickGUI") << "OpenGL debug logging disabled: error - the logger could not be ed";
        else
            msg_info("SofaQtQuickGUI") << "OpenGL debug logging enabled";

        connect(openglDebugLogger, &QOpenGLDebugLogger::messageLogged, [](const QOpenGLDebugMessage &debugMessage) {msg_info("SofaQtQuickGUI") << "OpenGL" << debugMessage.type() << "-" << "Severity;" << debugMessage.severity() << "- Source:" << debugMessage.source() <<  "- Message:" << debugMessage.message().toStdString();});
        openglDebugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);

        connect(ctx, &QOpenGLContext::aboutToBeDestroyed, [=]() {delete openglDebugLogger;});
    }
    else
    {
        msg_warning("SofaQtQuickGUI") << "OpenGL debug logging disabled: your graphics card does not support this functionality";
    }
}

void DefaultApplication::useDefaultSofaPath()
{
    // add the plugin directory to PluginRepository
#ifdef WIN32
    const std::string pluginDir = sofa::helper::Utils::getExecutableDirectory();
#else
    const std::string pluginDir = sofa::helper::Utils::getSofaPathPrefix() + "/lib";
#endif
    sofa::helper::system::PluginRepository.addFirstPath(pluginDir);

    sofa::helper::system::DataRepository.addFirstPath("../share/");
    sofa::helper::system::DataRepository.addFirstPath("../examples/");

    // read the paths to the share/ and examples/ directories from etc/sofa.ini

    const std::string etcDir = sofa::helper::Utils::getSofaPathPrefix() + "/etc";
    const std::string sofaIniFilePath = etcDir + "/sofa.ini";
    if(QFileInfo(QString::fromStdString(sofaIniFilePath)).exists())
    {
        std::map<std::string, std::string> iniFileValues = sofa::helper::Utils::readBasicIniFile(sofaIniFilePath);

        // and add them to DataRepository

        if(iniFileValues.find("SHARE_DIR") != iniFileValues.end())
        {
            std::string shareDir = iniFileValues["SHARE_DIR"];
            if (!sofa::helper::system::FileSystem::isAbsolute(shareDir))
                shareDir = etcDir + "/" + shareDir;
            sofa::helper::system::DataRepository.addFirstPath(shareDir);
        }

        if(iniFileValues.find("EXAMPLES_DIR") != iniFileValues.end())
        {
            std::string examplesDir = iniFileValues["EXAMPLES_DIR"];
            if (!sofa::helper::system::FileSystem::isAbsolute(examplesDir))
                examplesDir = etcDir + "/" + examplesDir;
            sofa::helper::system::DataRepository.addFirstPath(examplesDir);
        }
    }

    // also add application binary path
#ifndef WIN32
    sofa::helper::system::DataRepository.addFirstPath(QCoreApplication::applicationDirPath().toStdString());
#else
    sofa::helper::system::DataRepository.addFirstPath("./");
    sofa::helper::system::DataRepository.addFirstPath("../");
    sofa::helper::system::DataRepository.addFirstPath(QCoreApplication::applicationDirPath().toStdString() + "/../");
#endif
}

static QString ExitedNormallyKey()
{
    return "SofaApplication/ExitedNormally";
}

static QString CorruptedKey()
{
    return "SofaApplication/Corrupted";
}

void DefaultApplication::applySettings(const QString& defaultSettingsPath, const QString& backupSettingsPath)
{
    QSettings::setDefaultFormat(QSettings::IniFormat);

    connect(qApp, &QApplication::aboutToQuit,
            this, &DefaultApplication::finalizeSettings); // maybe too early ?

    QSettings settings;
    if(QFileInfo(settings.fileName()).exists())
    {
        bool exitedNormally = settings.value(ExitedNormallyKey(), false).toBool();
        if(exitedNormally && !settings.value(CorruptedKey(), false).toBool())
        {
            makeBackupSettings(backupSettingsPath);
            settings.remove(ExitedNormallyKey());
        }
        else
        {
            if(settings.value(CorruptedKey(), false).toBool())
                qCritical() << "Current settings has been corrupted, falling back on backup settings";

            applyBackupSettings(backupSettingsPath);
        }

        if(!settings.value(CorruptedKey(), false).toBool())
            return;
    }

    if(settings.value(CorruptedKey(), false).toBool())
        qCritical() << "Backup Settings file has been corrupted, falling back on default settings";

    applyDefaultSettings(defaultSettingsPath, backupSettingsPath);
}

void DefaultApplication::finalizeSettings()
{
    QSettings settings;
    if(!QFileInfo(settings.fileName()).exists())
    {
        qCritical("Cannot finalize settings, the application will use the backup settings at next launch");
        return;
    }

    settings.setValue(ExitedNormallyKey(), true);
}

static QString DefaultBackupSettingsPath()
{
    QSettings settings;
    return settings.fileName() + ".backup";
}

void DefaultApplication::makeBackupSettings(const QString& backupSettingsPath)
{
    QSettings settings;

    QString finalBackupSettingsPath = backupSettingsPath;
    if(finalBackupSettingsPath.isEmpty())
        finalBackupSettingsPath = DefaultBackupSettingsPath();

    QSettings backupSettings(finalBackupSettingsPath, QSettings::IniFormat);
    backupSettings.clear();

    copySettings(settings, backupSettings);
}

void DefaultApplication::applyBackupSettings(const QString& backupSettingsPath)
{
    msg_warning("SofaQtQuickGUI")  << "The previous instance of the application did not exit cleanly, falling back on the backup settings";

    QString finalBackupSettingsPath = backupSettingsPath;
    if(finalBackupSettingsPath.isEmpty())
        finalBackupSettingsPath = DefaultBackupSettingsPath();

    QSettings backupSettings(finalBackupSettingsPath, QSettings::IniFormat);
    QSettings settings;
    settings.clear();

    copySettings(backupSettings, settings);
}

void DefaultApplication::applyDefaultSettings(const QString& defaultSettingsPath, const QString& backupSettingsPath)
{
    QString finalDefaultSettingsPath = ":/config/default.ini";
    if(!defaultSettingsPath.isEmpty())
        finalDefaultSettingsPath = defaultSettingsPath;

    // copy properties of default.ini into the backup and current settings if the settings file does not exist
    QSettings defaultSettings(finalDefaultSettingsPath, QSettings::IniFormat);
    QSettings settings;
    settings.clear();

    copySettings(defaultSettings, settings);
    makeBackupSettings(backupSettingsPath);
}

static void SettingsCopyValuesHelper(const QSettings& src, QSettings& dst)
{
    QStringList keys = src.childKeys();
    foreach(QString key, keys)
        dst.setValue(key, src.value(key));
}

static void SettingsCopyGroupsHelper(const QSettings& src, QSettings& dst)
{
    QStringList groups = src.childGroups();
    foreach(QString group, groups)
    {
        const_cast<QSettings&>(src).beginGroup(group);

        dst.beginGroup(group);
        SettingsCopyGroupsHelper(src, dst);
        dst.endGroup();

        const_cast<QSettings&>(src).endGroup();
    }

    SettingsCopyValuesHelper(src, dst);
}

void DefaultApplication::copySettings(const QSettings& src, QSettings& dst)
{
    SettingsCopyGroupsHelper(src, dst);
}

bool DefaultApplication::surfaceDefinition()
{
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(5);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    return true;
}

void DefaultApplication::instanciate(QQmlApplicationEngine& applicationEngine)
{
    // this will instanciate a SofaApplication QML instance
    applicationEngine.loadData("import QtQuick 2.0\n"
                               "import SofaApplication 1.0\n"
                               "QtObject {\n"
                               "Component.onCompleted: {SofaApplication.objectName;}\n"
                               "}");
}

void DefaultApplication::destruction()
{
    qApp->quit();
}

bool DefaultApplication::mainInitialization(QApplication& app,
                                  QQmlApplicationEngine &applicationEngine,
                                  const QString& mainScript)
{
    // application style
    QQuickStyle::setStyle("Material");

    // color console
    sofa::helper::Console::setColorsStatus(sofa::helper::Console::ColorsEnabled);

    // TODO: this command disable the multithreaded render loop, currently we need this because our implementation of the sofa interface is not thread-safe
    qputenv("QSG_RENDER_LOOP", "basic");

    if(!app.testAttribute(Qt::AA_ShareOpenGLContexts))
        qCritical() << "CRITICAL: DefaultApplication::initialization() must be called before QApplication instanciation";

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

    // compute command line arguments
    QCommandLineParser parser;

    QCommandLineOption sceneOption(QStringList() << "s" << "scene", "Load with this scene", "file");
    QCommandLineOption animateOption(QStringList() << "a" << "animate", "Start in animate mode");
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", "Fullscreen mode");
    QCommandLineOption widthOption(QStringList() << "width", "Window width", "pixels");
    QCommandLineOption heightOption(QStringList() << "height", "Window height", "pixels");
    QCommandLineOption logTimeOption(QStringList() << "log", "Log time during simulation");

    parser.addOption(sceneOption);
    parser.addOption(animateOption);
    parser.addOption(fullscreenOption);
    parser.addOption(widthOption);
    parser.addOption(heightOption);
    parser.addOption(logTimeOption);

    parser.addVersionOption();
    parser.addHelpOption();

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

    if(parser.isSet("log")) {
        sofa::helper::AdvancedTimer::setEnabled("Animate", true);
    }

    // apply the app settings or use the default.ini settings if it is the
    // first time the user launch the application or use the app.backup.ini in case of application crash
    applySettings();

    // launch the main script

    applicationEngine.addImportPath("qrc:/");
    applicationEngine.addImportPath(QCoreApplication::applicationDirPath() + "/../lib/qml/");
    applicationEngine.load(QUrl(mainScript));

    // apply command line arguments

    QList<QObject*> objects = applicationEngine.rootObjects();
    foreach(QObject* object, objects)
    {
        QWindow* window = qobject_cast<QWindow*>(object);
        if(!window)
            continue;

        if(parser.isSet(animateOption) || parser.isSet(sceneOption))
        {
            SofaScene* sofaScene = object->findChild<SofaScene*>();
            if(parser.isSet(sceneOption))
            {
                sofaScene->setSource(parser.value(sceneOption));
            }
            if(parser.isSet(animateOption))
            {
                sofaScene->setDefaultAnimate(true);
                if(sofaScene->isReady())
                    sofaScene->setAnimate(true);
            }
        }

        if(parser.isSet(widthOption))
            window->setWidth(parser.value(widthOption).toInt());

        if(parser.isSet(heightOption))
            window->setHeight(parser.value(heightOption).toInt());

        if(parser.isSet(fullscreenOption))
            window->setVisibility(QWindow::FullScreen);
    }

    return true;
}

} // qtquick
} // sofa
