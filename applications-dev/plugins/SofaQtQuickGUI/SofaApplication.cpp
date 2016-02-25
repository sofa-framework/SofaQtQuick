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

#include "SofaApplication.h"
#include "SofaScene.h"

#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/Utils.h>
#include <sofa/helper/system/console.h>

#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlContext>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QPluginLoader>
#include <QSettings>
#include <QScreen>
#include <QDir>
#include <QDebug>
#include <QCommandLineParser>
#include <QWindow>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <signal.h>
#endif

namespace sofa
{

namespace qtquick
{

SofaApplication* SofaApplication::OurInstance = nullptr;

SofaApplication::SofaApplication(QObject* parent) : QObject(parent)
{
    OurInstance = this;
}

SofaApplication::~SofaApplication()
{
    if(this == OurInstance)
        OurInstance = nullptr;
}

SofaApplication* SofaApplication::Instance()
{
    return OurInstance;
}

QString SofaApplication::binaryDirectory() const
{
    return QCoreApplication::applicationDirPath();
}

void SofaApplication::saveScreenshot(const QString& path)
{
    if(!qGuiApp)
    {
        qWarning() << "Cannot take a screenshot of the whole application without an instance of QGuiApplication";
        return;
    }

    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if(!dir.exists())
        dir.mkpath(".");

    QString newPath = fileInfo.absoluteDir().absolutePath() + "/";
    newPath += fileInfo.baseName();
    QString suffix = "." + fileInfo.completeSuffix();

    QWindowList windows = qGuiApp->allWindows();
    QList<QQuickWindow*> quickWindows;
    for(QWindow* window : windows)
    {
        QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(window);
        if(quickWindow)
            quickWindows.append(quickWindow);
    }

    int count = -1;
    if(quickWindows.size() > 1)
        count = 0;

    bool saved = false;
    for(QQuickWindow* quickWindow : quickWindows)
    {
        QString finalPath = newPath;
        if(-1 != count)
            finalPath += "_" + QString::number(count++);

        finalPath += suffix;

        if(quickWindow->grabWindow().save(finalPath))
            saved = true;
    }

    if(!saved)
        qWarning() << "Screenshot could not be saved to" << path;
}

int SofaApplication::overrideCursorShape() const
{
    return QApplication::overrideCursor() ? QApplication::overrideCursor()->shape() : Qt::ArrowCursor;
}

void SofaApplication::setOverrideCursorShape(int newCursorShape)
{
    if(newCursorShape == overrideCursorShape())
        return;

    QApplication::restoreOverrideCursor();
    if(Qt::ArrowCursor != newCursorShape)
        QApplication::setOverrideCursor(QCursor((Qt::CursorShape) newCursorShape));

    overrideCursorShapeChanged();
}

QPointF SofaApplication::mapItemToScene(QQuickItem* item, const QPointF& point) const
{
    if(!item)
        return QPointF();

    return item->mapToScene(point);
}

bool SofaApplication::isAncestorItem(QQuickItem* item, QQuickItem* ancestorItem) const
{
    if(item && ancestorItem && item != ancestorItem)
        for(QQuickItem* parentItem = item->parentItem(); parentItem; parentItem = parentItem->parentItem())
            if(ancestorItem == parentItem)
                return true;

    return false;
}

QQuickWindow* SofaApplication::itemWindow(QQuickItem* item) const
{
    if(!item)
        return nullptr;

    return item->window();
}

void SofaApplication::trimComponentCache(QObject* object)
{
    if(!object)
        object = this;

    QQmlContext* context = QQmlEngine::contextForObject(object);
    if(!context)
        return;

    QQmlEngine* engine = context->engine();
    if(!engine)
        return;

    engine->trimComponentCache();
}

void SofaApplication::clearSettingGroup(const QString& group)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.remove("");
    settings.endGroup();
}

void SofaApplication::SetOpenGLDebugContext()
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

void SofaApplication::UseOpenGLDebugLogger()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if(0 == ctx) {
        qWarning() << "SofaApplication::initializeDebugLogger has been called without a valid opengl context made current";
        return;
    }

    qInfo() << "OpenGL Context" << (QString::number(ctx->format().majorVersion()) + "." + QString::number(ctx->format().minorVersion())).toLatin1().constData();
    qInfo() << "Graphics Card Vendor:" << (char*) glGetString(GL_VENDOR);
    qInfo() << "Graphics Card Model:" << (char*) glGetString(GL_RENDERER);
    qInfo() << "Graphics Card Drivers:" << (char*) glGetString(GL_VERSION);

    if(ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
    {
        QOpenGLDebugLogger* openglDebugLogger = new QOpenGLDebugLogger();
        if(!openglDebugLogger->initialize())
            qWarning() << "OpenGL debug logging disabled: error - the logger could not be initialized";
        else
            qInfo() << "OpenGL debug logging enabled";

        connect(openglDebugLogger, &QOpenGLDebugLogger::messageLogged, [](const QOpenGLDebugMessage &debugMessage) {qInfo() << "OpenGL" << debugMessage.type() << "-" << "Severity;" << debugMessage.severity() << "- Source:" << debugMessage.source() <<  "- Message:" << debugMessage.message();});
        openglDebugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);

        connect(ctx, &QOpenGLContext::aboutToBeDestroyed, [=]() {delete openglDebugLogger;});
    }
    else
    {
        qWarning() << "OpenGL debug logging disabled: your graphics card does not support this functionality";
    }
}

void SofaApplication::UseDefaultSofaPath()
{
    // add the plugin directory to PluginRepository
#ifdef WIN32
    const std::string pluginDir = sofa::helper::Utils::getExecutableDirectory();
#else
    const std::string pluginDir = sofa::helper::Utils::getSofaPathPrefix() + "/lib";
#endif
    sofa::helper::system::PluginRepository.addFirstPath(pluginDir);

    // read the paths to the share/ and examples/ directories from etc/sofa.ini

    const std::string etcDir = sofa::helper::Utils::getSofaPathPrefix() + "/etc";
    const std::string sofaIniFilePath = etcDir + "/sofa.ini";
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

void SofaApplication::ApplySettings(const QString& defaultSettingsPath, const QString& backupSettingsPath)
{
    QSettings::setDefaultFormat(QSettings::IniFormat);

    connect(qApp, &QApplication::aboutToQuit, &SofaApplication::FinalizeSettings); // maybe too early ?

    QSettings settings;
    if(QFileInfo(settings.fileName()).exists())
    {
        bool exitedNormally = settings.value(ExitedNormallyKey(), false).toBool();
        if(exitedNormally && !settings.value(CorruptedKey(), false).toBool())
        {
            MakeBackupSettings(backupSettingsPath);
            settings.remove(ExitedNormallyKey());
        }
        else
        {
            if(settings.value(CorruptedKey(), false).toBool())
                qCritical() << "Current settings has been corrupted, falling back on backup settings";

            ApplyBackupSettings(backupSettingsPath);
        }

        if(!settings.value(CorruptedKey(), false).toBool())
            return;
    }

    if(settings.value(CorruptedKey(), false).toBool())
        qCritical() << "Backup Settings file has been corrupted, falling back on default settings";

    ApplyDefaultSettings(defaultSettingsPath, backupSettingsPath);
}

void SofaApplication::FinalizeSettings()
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

void SofaApplication::MakeBackupSettings(const QString& backupSettingsPath)
{
    QSettings settings;

    QString finalBackupSettingsPath = backupSettingsPath;
    if(finalBackupSettingsPath.isEmpty())
        finalBackupSettingsPath = DefaultBackupSettingsPath();

    QSettings backupSettings(finalBackupSettingsPath, QSettings::IniFormat);
    backupSettings.clear();

    CopySettings(settings, backupSettings);
}

void SofaApplication::ApplyBackupSettings(const QString& backupSettingsPath)
{
    qWarning() << "The previous instance of the application did not exit cleanly, falling back on the backup settings";

    QString finalBackupSettingsPath = backupSettingsPath;
    if(finalBackupSettingsPath.isEmpty())
        finalBackupSettingsPath = DefaultBackupSettingsPath();

    QSettings backupSettings(finalBackupSettingsPath, QSettings::IniFormat);
    QSettings settings;
    settings.clear();

    CopySettings(backupSettings, settings);
}

void SofaApplication::ApplyDefaultSettings(const QString& defaultSettingsPath, const QString& backupSettingsPath)
{
    QString finalDefaultSettingsPath = ":/config/default.ini";
    if(!defaultSettingsPath.isEmpty())
        finalDefaultSettingsPath = defaultSettingsPath;

    // copy properties of default.ini into the backup and current settings if the settings file does not exist
    QSettings defaultSettings(finalDefaultSettingsPath, QSettings::IniFormat);
    QSettings settings;
    settings.clear();

    CopySettings(defaultSettings, settings);
    MakeBackupSettings(backupSettingsPath);
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

void SofaApplication::CopySettings(const QSettings& src, QSettings& dst)
{
    SettingsCopyGroupsHelper(src, dst);
}

bool SofaApplication::Initialization()
{
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    return true;
}

void SofaApplication::Destruction()
{
    qApp->quit();
}

bool SofaApplication::DefaultMain(QApplication& app, QQmlApplicationEngine &applicationEngine, const QString& mainScript)
{
    // color console
    sofa::helper::Console::setColorsStatus(sofa::helper::Console::ColorsEnabled);

    // TODO: this command disable the multithreaded render loop, currently we need this because our implementation of the sofa interface is not thread-safe
    qputenv("QSG_RENDER_LOOP", "basic");

    if(!app.testAttribute(Qt::AA_ShareOpenGLContexts))
        qCritical() << "CRITICAL: SofaApplication::initialization() must be called before QApplication instanciation";

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    // quit on Ctrl-C
    signal(SIGINT, [](int) {SofaApplication::Destruction();});
#endif

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    app.addLibraryPath(QCoreApplication::applicationDirPath() + "/../lib/");

    QSettings::setPath(QSettings::Format::IniFormat, QSettings::Scope::UserScope, QCoreApplication::applicationDirPath() + "/config/");

    // initialise paths
    SofaApplication::UseDefaultSofaPath();

    // use the default.ini settings if it is the first time the user launch the application or use the app.backup.ini in case of application crash
    SofaApplication::ApplySettings();

    // plugin initialization
    QString pluginName("SofaQtQuickGUI");
#ifndef NDEBUG
    pluginName += "_d";
#endif
    QPluginLoader pluginLoader(pluginName);

    // first call to instance() initialize the plugin
    if(0 == pluginLoader.instance()) {
        qCritical() << "ERROR: SofaQtQuickGUI plugin has not been found "<<pluginName;
        return false;
    }

    // launch the main script
    applicationEngine.addImportPath("qrc:/");
    applicationEngine.addImportPath(QCoreApplication::applicationDirPath() + "/../lib/qml/");
    applicationEngine.load(QUrl(mainScript));

    // use command line arguments
    QCommandLineParser parser;

    QCommandLineOption sceneOption(QStringList() << "s" << "scene", "Start the application with this scene", "file");
    QCommandLineOption animateOption(QStringList() << "a" << "animate", "Start the application in animate mode");
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", "Start the application in fullscreen mode");

    parser.addOption(sceneOption);
    parser.addOption(animateOption);
    parser.addOption(fullscreenOption);

    parser.parse(app.arguments());

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

        if(parser.isSet(fullscreenOption))
            window->setVisibility(QWindow::FullScreen);
    }

    return true;
}

}

}
