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

#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/Utils.h>

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

    qDebug() << "OpenGL Context" << (QString::number(ctx->format().majorVersion()) + "." + QString::number(ctx->format().minorVersion())).toLatin1().constData();
    qDebug() << "Graphics Card Vendor:" << (char*) glGetString(GL_VENDOR);
    qDebug() << "Graphics Card Model:" << (char*) glGetString(GL_RENDERER);
    qDebug() << "Graphics Card Drivers:" << (char*) glGetString(GL_VERSION);

    if(ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
    {
        QOpenGLDebugLogger* openglDebugLogger = new QOpenGLDebugLogger();
        if(!openglDebugLogger->initialize())
            qDebug() << "OpenGL debug logging disabled: error - the logger could not be initialized";
        else
            qDebug() << "OpenGL debug logging enabled";

        connect(openglDebugLogger, &QOpenGLDebugLogger::messageLogged, [](const QOpenGLDebugMessage &debugMessage) {qDebug() << "OpenGL" << debugMessage.type() << "-" << "Severity;" << debugMessage.severity() << "- Source:" << debugMessage.source() <<  "- Message:" << debugMessage.message();});
        openglDebugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);

        connect(ctx, &QOpenGLContext::aboutToBeDestroyed, [=]() {delete openglDebugLogger;});
    }
    else
    {
        qDebug() << "OpenGL debug logging disabled: your graphics card does not support this functionality";
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
#endif
}

void SofaApplication::UseDefaultSettingsAtFirstLaunch(const QString& defaultSettingsPath)
{
    QSettings settings;
    bool notFirstLaunch = settings.value("notFirstLaunch", false).toBool();
    if(notFirstLaunch)
        return;

    // copy default.ini into the current directory to be able to open it with QSettings
    QString defaultConfigFilePath = "default.ini";

    QFileInfo fileInfo(defaultConfigFilePath);
    if(!fileInfo.isFile())
    {
        QString finalDefaultSettingsPath = defaultSettingsPath;
        if(finalDefaultSettingsPath.isEmpty())
            finalDefaultSettingsPath = ":/config/default.ini";

        QFile file(finalDefaultSettingsPath);
        if(!file.open(QFile::OpenModeFlag::ReadOnly))
        {
            qWarning() << "ERROR: the default config file has not been found!";
        }
        else
        {
            if(!file.copy(defaultConfigFilePath))
                qWarning() << "ERROR: the config file could not be created!";
            else
                if(!QFile::setPermissions(defaultConfigFilePath, QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup))
                    qWarning() << "ERROR: cannot set permission on the config file!";
        }
    }

    // copy properties of default.ini into the current settings
    QSettings defaultSettings(defaultConfigFilePath, QSettings::IniFormat);
    CopySettings(defaultSettings, settings);

    settings.setValue("notFirstLaunch", true);
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

bool SofaApplication::DefaultMain(QApplication& app, QQmlApplicationEngine &applicationEngine, const QString& mainScript)
{
    // TODO: this command disable the multithreaded render loop, currently we need this because our implementation of the sofa interface is not thread-safe
    qputenv("QSG_RENDER_LOOP", "basic");

    if(!app.testAttribute(Qt::AA_ShareOpenGLContexts))
        qCritical() << "CRITICAL: SofaApplication::initialization() must be called before QApplication instanciation";

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    app.addLibraryPath(QCoreApplication::applicationDirPath() + "/../lib/");

    QSettings::setPath(QSettings::Format::IniFormat, QSettings::Scope::UserScope, QCoreApplication::applicationDirPath() + "/config/");
    QSettings::setDefaultFormat(QSettings::Format::IniFormat);

    // initialise paths
    SofaApplication::UseDefaultSofaPath();

    // use the default.ini settings if it is the first time the user launch the application
    SofaApplication::UseDefaultSettingsAtFirstLaunch();

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

    //    QList<QObject*> objects = applicationEngine.rootObjects();
    //    foreach(QObject* object, objects)
    //    {
    //        QQuickWindow* window = qobject_cast<QQuickWindow*>(object);
    //        if(!window)
    //            continue;

    //        window->connect(window, &QQuickWindow::openglContextCreated, [](QOpenGLContext *context) {
    //            qDebug() << "opengl context creation";
    //        });
    //        window->show();
    //    }

    return true;
}

}

}
