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
#include <QDirIterator>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <signal.h>
#endif

namespace sofa
{

namespace qtquick
{

using namespace sofa::helper;
using namespace sofa::simulation;

SofaApplication* SofaApplication::OurInstance = nullptr;

SofaApplication::SofaApplication(QObject* parent) : QObject(parent),
	myPythonDirectory(),
    myDataDirectory()
{
    OurInstance = this;

    // look for the data directory next to the executable location
    if(QCoreApplication::applicationName().isEmpty())
        qCritical() << "The QCoreApplication::applicationName should have been setted before SofaApplication instantiation";

    QVector<QString> paths;

    paths.append(QCoreApplication::applicationDirPath() + "/../data");
    paths.append(QCoreApplication::applicationDirPath() + "/../" + QCoreApplication::applicationName() + "Data");
    paths.append(QCoreApplication::applicationDirPath() + "/" + QCoreApplication::applicationName() + "Data");
    paths.append(QCoreApplication::applicationDirPath() + "/data");

    for(const QString& path : paths)
        if(QFileInfo::exists(path))
        {
            myDataDirectory = path + "/";
            break;
        }
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

void SofaApplication::copyToClipboard(const QString& text)
{
    QClipboard* clipboard = QApplication::clipboard();
    if(!clipboard)
        return;

    clipboard->setText(text);
}

void SofaApplication::openInExplorer(const QString& folder) const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
}

bool SofaApplication::createFolder(const QString& destination)
{
	QDir dir(destination);
	if (!dir.exists())
		return dir.mkpath(".");

	return false;
}

bool SofaApplication::removeFolder(const QString& destination)
{
    QDir dir(destination);
    return dir.removeRecursively();
}

bool SofaApplication::copyFolder(const QString& source, const QString& destination)
{
	QDir destinationDir(destination);
	if (destinationDir.exists())
		return false;

	QDir baseSourceDir(source);

	QStack<QDir> sourceDirs;
	sourceDirs.push(baseSourceDir);

	while (!sourceDirs.isEmpty())
	{
		QDir sourceDir = sourceDirs.pop();

		QFileInfoList entries = sourceDir.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDir::DirsFirst);
		for (QFileInfo entry : entries)
		{
			if (entry.isDir())
			{
				sourceDirs.push(QDir(entry.filePath()));
			}
			else if (entry.isFile())
			{
				QString relativeFolderPath = destination + "/" + baseSourceDir.relativeFilePath(entry.absolutePath());
				QDir().mkpath(relativeFolderPath);

				QString sourceFilepath = entry.filePath();
				QString destinationFilepath = destination + "/" + baseSourceDir.relativeFilePath(entry.filePath());

				QFile(sourceFilepath).copy(destinationFilepath);
			}
		}
	}

	return true;
}

QStringList SofaApplication::findFiles(const QString& dirPath, const QStringList& nameFilters)
{
	QStringList filepaths;

	QDir dir(dirPath);
	if (!dir.exists())
		return filepaths;

	QFileInfoList entries = dir.entryInfoList(nameFilters, QDir::Files);
	for (QFileInfo entry : entries)
		filepaths << entry.filePath();

	return filepaths;
}

QString SofaApplication::loadFile(const QString& filename)
{
    QFile file(filename);
    if(file.open(QFile::ReadOnly))
        return QString(file.readAll());

    return QString();
}

bool SofaApplication::saveFile(const QString& destination, const QString& data)
{
    QFileInfo fileInfo(destination);
    QDir dir = fileInfo.dir();
    if (!dir.exists())
        dir.mkpath(".");

    QFile file(destination);
    if(!file.open(QFile::WriteOnly))
        return false;

    QTextStream out(&file);
    out << data;

    return true;
}

bool SofaApplication::copyFile(const QString& source, const QString& destination)
{
	QFileInfo fileInfo(destination);
	if (fileInfo.isDir())
	{
		return QFile::copy(source, destination + "/" + QFileInfo(source).fileName());
	}
	else
	{
		QDir dir = fileInfo.dir();
		if (dir.exists())
			QFile(destination).remove();
		else
			dir.mkpath(".");

		return QFile::copy(source, destination);
	}
}

QImage SofaApplication::screenshotComponent(QQuickItem* item, const QSize& forceSize) const
{
    if(!item)
    {
        msg_error("SofaApplication::screenshotComponent") << "QQuickItem is null";
        return QImage();
    }

    QSize itemSize(item->width(), item->height());
    QSize itemScreenshotSize = itemSize;
    if(forceSize.isValid())
        itemScreenshotSize = forceSize;

    item->setSize(itemScreenshotSize);

    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);

    QOpenGLContext* glCurrentContext = QOpenGLContext::currentContext();

    QOpenGLContext* glContext = glCurrentContext;
    if(!glContext)
    {
        glContext = new QOpenGLContext;
        glContext->setFormat(format);
        glContext->create();

        QOffscreenSurface* offscreenSurface = new QOffscreenSurface;
        offscreenSurface->setParent(glContext);
        offscreenSurface->setFormat(glContext->format());
        offscreenSurface->create();

        glContext->makeCurrent(offscreenSurface);
    }

    QQuickRenderControl renderControl;
    QQuickWindow window(&renderControl);

    item->setParentItem(window.contentItem());

    window.setGeometry(0, 0, itemScreenshotSize.width(), itemScreenshotSize.height());

    renderControl.initialize(glContext);

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setSamples(4);
    QOpenGLFramebufferObject fbo(window.size() * window.effectiveDevicePixelRatio(), fboFormat);

    window.setRenderTarget(&fbo);

    renderControl.polishItems();
    renderControl.sync();
    renderControl.render();

    QImage image = fbo.toImage();

    renderControl.invalidate();

    if(glContext != glCurrentContext)
        delete glContext;

    if(forceSize.isValid())
        item->setSize(itemSize);

    return image;
}

QImage SofaApplication::screenshotComponent(QQmlComponent* component, const QSize& forceSize) const
{
    if(!component)
    {
        msg_error("SofaApplication::screenshotComponent") << "QQmlComponent is null";
        return QImage();
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(component->create());

    return screenshotComponent(item, forceSize);
}

QImage SofaApplication::screenshotComponent(const QUrl& componentUrl, const QSize& forceSize) const
{
    QQmlEngine* engine = qmlEngine(this);

    QQmlComponent component(engine, componentUrl);
    if(component.status() != QQmlComponent::Ready)
    {
        msg_error("SofaApplication::screenshotComponent") << "QQmlComponent is not ready, errors are:" << component.errorString().toStdString();
        return QImage();
    }

    return screenshotComponent(&component, forceSize);
}

bool SofaApplication::screenshotComponent(QQuickItem* item, const QString& destination, const QSize& forceSize) const
{
    QImage image = screenshotComponent(item, forceSize);

    if(!image.save(destination))
    {
        msg_error("SofaApplication::screenshotComponent") << "Image could not be saved, possible reasons are : item has an incorrect size or the image destination path is incorrect";
        return false;
    }

    return true;
}

bool SofaApplication::screenshotComponent(QQmlComponent* component, const QString& destination, const QSize& forceSize) const
{
    if(!component)
    {
        msg_error("SofaApplication::screenshotComponent") << "QQmlComponent is null";
        return false;
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(component->create());

    return screenshotComponent(item, destination, forceSize);
}

bool SofaApplication::screenshotComponent(const QUrl& componentUrl, const QString& destination, const QSize& forceSize) const
{
    QQmlEngine* engine = qmlEngine(this);

    QQmlComponent component(engine, componentUrl);
    if(component.status() != QQmlComponent::Ready)
    {
        msg_error("SofaApplication::screenshotComponent") << "QQmlComponent is not ready, errors are:" << component.errorString().toStdString();
        return false;
    }

    return screenshotComponent(&component, destination, forceSize);
}

bool SofaApplication::runPythonScript(const QString& script)
{
    return PythonEnvironment::runString(script.toStdString());
}

bool SofaApplication::runPythonFile(const QString& filename)
{
    return PythonEnvironment::runFile(filename.toLatin1().constData());
}

QVariantList SofaApplication::executeProcess(const QString& command, int timeOutMsecs)
{
    QProcess process;
    process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process.setProcessChannelMode(QProcess::ForwardedChannels); // to display stdout/sterr (QProcess::MergedChannels for stdout only)

    process.start(command);

    process.waitForFinished(timeOutMsecs);

    return QVariantList() << QVariant::fromValue((int) process.exitStatus()) << QVariant::fromValue(process.exitCode()) << QVariant::fromValue(process.readAllStandardOutput()) << QVariant::fromValue(process.readAllStandardError());
}

ProcessState* SofaApplication::executeProcessAsync(const QString& command)
{
    QProcess* process = new QProcess();
    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process->setProcessChannelMode(QProcess::ForwardedChannels); // to display stdout/sterr (QProcess::MergedChannels for stdout only)

    ProcessState* processState = new ProcessState(process);

    process->start(command);

    return processState;

    // do not forget to call destroyProcess to delete "process"
    // how to delete "processState"?
}

void SofaApplication::addNextFrameAction(QJSValue& jsFunction)
{
	if(!jsFunction.isCallable())
	{
		qWarning() << "SofaApplication::addNextFrameAction - parameter must be a JS function";
		return;
	}

	QTimer::singleShot(0, [&]() { jsFunction.call(); });
}

QString SofaApplication::pythonDirectory() const
{
	return myPythonDirectory;
}

void SofaApplication::setPythonDirectory(const QString& pythonDirectory)
{
	myPythonDirectory = pythonDirectory;
}

QString SofaApplication::dataDirectory() const
{
    return myDataDirectory;
}

void SofaApplication::setDataDirectory(const QString& dataDirectory)
{
    myDataDirectory = dataDirectory;

    if(!myDataDirectory.isEmpty() && !(myDataDirectory.endsWith("/") || myDataDirectory.endsWith("\\")))
        myDataDirectory += "/";
}

QString SofaApplication::binaryDirectory() const
{
    return QCoreApplication::applicationDirPath() + "/";
}

void SofaApplication::saveScreenshot(const QString& path)
{
    if(!qGuiApp)
    {
        msg_error("SofaQtQuickGUI") << "Cannot take a screenshot of the whole application without an instance of QGuiApplication";
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
        msg_error("SofaQtQuickGUI") << "Screenshot could not be saved to " << path.toStdString();
}

QObject* SofaApplication::instanciateURL(const QUrl& url)
{
    QQmlEngine* engine = qmlEngine(this);

    QQmlComponent component(engine, url, this);
    if(component.isReady())
    {
        QObject* object = component.create(qmlContext(this));
        return object;
    }

    msg_error("SofaQtQuickGUI") << "Component could not be instanciated, reason: " << component.errorString().toStdString();

    return nullptr;
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

int SofaApplication::objectDepthFromRoot(QObject* object)
{
    int depth = -1;

    if(object)
        for(QObject* ancestor = object->parent(); ancestor; ancestor = ancestor->parent())
            ++depth;

    return depth;
}

QString SofaApplication::toLocalFile(const QUrl& url)
{
    return url.toLocalFile();
}

QQuaternion SofaApplication::quaternionFromEulerAngles(const QVector3D& eulerAngles) const
{
    return QQuaternion::fromEulerAngles(eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
}

QVector3D SofaApplication::quaternionToEulerAngles(const QQuaternion& quaternion) const
{
    return quaternion.toEulerAngles();
}

QVariantList SofaApplication::quaternionToAxisAngle(const QQuaternion& quaternion) const // return [QVector3D axis, float angle];
{
    QVector3D axis;
    float angle = 0.0f;

    quaternion.getAxisAndAngle(&axis, &angle);

    return QVariantList() << QVariant::fromValue(axis) << QVariant::fromValue(angle);
}

QQuaternion SofaApplication::quaternionFromAxisAngle(const QVector3D& axis, float angle) const
{
    return QQuaternion::fromAxisAndAngle(axis, angle);
}

QQuaternion SofaApplication::quaternionDifference(const QQuaternion& q0, const QQuaternion& q1) const
{
    return q1 * q0.conjugated();
}

QQuaternion SofaApplication::quaternionMultiply(const QQuaternion& q0, const QQuaternion& q1) const
{
    return q0 * q1;
}

QQuaternion SofaApplication::quaternionConjugate(const QQuaternion& q) const
{
    return q.conjugated();
}

QVector3D SofaApplication::quaternionRotate(const QQuaternion& q, const QVector3D& v) const
{
    return q.rotatedVector(v);
}

/**
   Decompose the q rotation on to 2 parts.
   1. Twist - rotation around the "direction" vector
   2. Swing - rotation around axis that is perpendicular to "direction" vector
   The rotation can be composed back by
   rotation = swing * twist

   has singularity in case of swing_rotation close to 180 degrees rotation.
   if the input quaternion is of non-unit length, the outputs are non-unit as well
   otherwise, outputs are both unit
*/
QVariantList SofaApplication::quaternionSwingTwistDecomposition(const QQuaternion& q, const QVector3D& direction)
{
    QVector3D ra(q.x(), q.y(), q.z()); // rotation axis
    QVector3D p = direction.normalized() * QVector3D::dotProduct(ra, direction.normalized()); // projection( ra, direction ); // return projection v1 on to v2  (parallel component)

    QQuaternion twist(q.scalar(), p.x(), p.y(), p.z());
    twist.normalize();

    QQuaternion swing = q * twist.conjugated();

    return QVariantList() << QVariant::fromValue(swing) << QVariant::fromValue(twist);
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
        msg_error("SofaQtQuickGUI") << "SofaApplication::initializeDebugLogger has been called without a valid opengl context made current";
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
            msg_error("SofaQtQuickGUI") << "OpenGL debug logging disabled: error - the logger could not be initialized";
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

void SofaApplication::UseDefaultSofaPath()
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
    msg_warning("SofaQtQuickGUI")  << "The previous instance of the application did not exit cleanly, falling back on the backup settings";

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
	BackTrace::autodump();

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

void SofaApplication::Instanciate(QQmlApplicationEngine& applicationEngine)
{
    // this will instanciate a SofaApplication QML instance
    applicationEngine.loadData("import QtQuick 2.0\n"
                               "import SofaApplication 1.0\n"
                               "QtObject {\n"
                               "Component.onCompleted: {SofaApplication.objectName;}\n"
                               "}");
}

void SofaApplication::Destruction()
{
    qApp->quit();
}

class UseOpenGLDebugLoggerRunnable : public QRunnable
{
	void run()
	{
		SofaApplication::UseOpenGLDebugLogger();
	}
};

bool SofaApplication::DefaultMain(QApplication& app, QQmlApplicationEngine &applicationEngine, const QString& mainScript)
{
    // application style
    QQuickStyle::setStyle("Material");

    // color console
    //sofa::helper::console::setStatus(sofa::helper::console::Status::On);

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
    
    // initialise paths
    SofaApplication::UseDefaultSofaPath();

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
    QCommandLineOption guiConfigOption(QStringList() << "gc" << "guiconfig", "Load with this GUI configuration (absolute path to an ini file OR a config name)", "guiconfig");
    QCommandLineOption animateOption(QStringList() << "a" << "animate", "Start in animate mode");
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", "Fullscreen mode");
    QCommandLineOption widthOption(QStringList() << "width", "Window width", "pixels");
    QCommandLineOption heightOption(QStringList() << "height", "Window height", "pixels");
    QCommandLineOption logTimeOption(QStringList() << "log", "Log time during simulation");
    QCommandLineOption extraParamsOption(QStringList() << "pyargs", "Forward extra parameters for Python interpreter", "args...");
    
    parser.addOption(sceneOption);
    parser.addOption(guiConfigOption);
    parser.addOption(animateOption);
    parser.addOption(fullscreenOption);
    parser.addOption(widthOption);
    parser.addOption(heightOption);
    parser.addOption(logTimeOption);
    parser.addOption(extraParamsOption);
    
    parser.addVersionOption();
    parser.addHelpOption();

    QStringList args = app.arguments();
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

    bool optionConfigFile = false;

    //default config file
    QString configPath = QCoreApplication::applicationDirPath() + "/config/";
    //apply the app settings or use the default.ini settings if it is the first time the user launch the application or use the app.backup.ini in case of application crash
    QSettings::setPath(QSettings::Format::IniFormat, QSettings::Scope::UserScope, configPath);

    // if given as argument, use a given filepath as configuration
    if(parser.isSet(guiConfigOption))
    {
        QString guiConfigOptionValue = parser.value(guiConfigOption);
        //first search for absolute path
        QFileInfo fileInfo(guiConfigOptionValue);
        if(fileInfo.exists())
        {
            configPath = fileInfo.absoluteFilePath();
            optionConfigFile = true;
        }
        else
        {
            //second search in the config template dir (with or without the .ini suffix)
            QString qrcPath = ":/config/";
            QString configName = guiConfigOptionValue.section(".", 0, 0) + ".ini";
            qDebug() << qrcPath + configName;
            QFileInfo fileInfo2(qrcPath + configName);
            if (fileInfo2.exists())
            {
                configPath = fileInfo2.absoluteFilePath();
                optionConfigFile = true;
            }
            else
            {
                msg_error("SofaQtQuickGUI:") << "Could not find any config from the argument " << guiConfigOptionValue.toStdString();
            }
        }
    }

    SofaApplication::ApplySettings();

    if (optionConfigFile)
    {
        //copy content of the given config file into the "system", as this one cant be changed later on
        QSettings optionConfigSettings(configPath, QSettings::Format::IniFormat);
        QSettings settings;
        settings.clear();

        CopySettings(optionConfigSettings, settings);
    }

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

		//QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(window);
		//if (quickWindow)
		//	quickWindow->scheduleRenderJob(new UseOpenGLDebugLoggerRunnable(), QQuickWindow::RenderStage::BeforeRenderingStage);

		if(parser.isSet(animateOption) || parser.isSet(sceneOption))
        {
            SofaScene* sofaScene = object->findChild<SofaScene*>();
            if(parser.isSet(extraParamsOption))
            {
                std::vector<std::string> extraArgs;
                auto it = args.begin();
                while (*it != "--pyargs") ++it;
                ++it;
                while (it != args.end()) {
                    std::string arg = (*it).toStdString();
                    extraArgs.push_back(arg);
                    ++it;
                }

                sofaScene->setExtraParams(extraArgs);
            }
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

}

}
