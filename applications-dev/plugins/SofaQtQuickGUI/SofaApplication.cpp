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

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <signal.h>
#endif

namespace sofa
{

namespace qtquick
{

using namespace sofa::helper;
using namespace sofa::simulation;

SofaApplication* MainApplication::m_maininstance = nullptr;

MainApplication::MainApplication(QObject* parent) : QObject(parent),
    myPythonDirectory(),
    myDataDirectory()
{
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

MainApplication::~MainApplication()
{
}

void MainApplication::copyToClipboard(const QString& text)
{
    QClipboard* clipboard = QApplication::clipboard();
    if(!clipboard)
        return;

    clipboard->setText(text);
}

void MainApplication::openInExplorer(const QString& folder) const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
}

bool MainApplication::createFolder(const QString& destination)
{
    QDir dir(destination);
    if (!dir.exists())
        return dir.mkpath(".");

    return false;
}

bool MainApplication::removeFolder(const QString& destination)
{
    QDir dir(destination);
    return dir.removeRecursively();
}

bool MainApplication::copyFolder(const QString& source, const QString& destination)
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

QStringList MainApplication::findFiles(const QString& dirPath, const QStringList& nameFilters)
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

QString MainApplication::loadFile(const QString& filename)
{
    QFile file(filename);
    if(file.open(QFile::ReadOnly))
        return QString(file.readAll());

    return QString();
}

bool MainApplication::saveFile(const QString& destination, const QString& data)
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

bool MainApplication::copyFile(const QString& source, const QString& destination)
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

bool MainApplication::screenshotComponent(const QUrl& componentUrl, const QString& destination)
{
    QQmlComponent component(qmlEngine(this), componentUrl);
    if(component.status() != QQmlComponent::Ready)
    {
        msg_error("AteosApplication::savePatientInfoInImage") << "QmlComponent is not ready, errors are:" << component.errorString().toStdString();
        return false;
    }

    QQuickItem* content = qobject_cast<QQuickItem*>(component.create());
    if(!content)
    {
        msg_error("AteosApplication::savePatientInfoInImage") << "QmlComponent should embed an object inheriting QQuickItem";
        return false;
    }

    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);

    QOpenGLContext* glContext = new QOpenGLContext;
    glContext->setFormat(format);
    glContext->create();

    QOffscreenSurface* offscreenSurface = new QOffscreenSurface;
    offscreenSurface->setFormat(glContext->format());
    offscreenSurface->create();

    glContext->makeCurrent(offscreenSurface);

    QQuickRenderControl renderControl;
    QQuickWindow window(&renderControl);

    content->setParentItem(window.contentItem());

    window.setGeometry(0, 0, content->implicitWidth(), content->implicitHeight());

    renderControl.initialize(glContext);

    QOpenGLFramebufferObjectFormat fboFormat;
    fboFormat.setSamples(4);
    QOpenGLFramebufferObject fbo(window.size() * window.effectiveDevicePixelRatio(), fboFormat);

    window.setRenderTarget(&fbo);

    renderControl.polishItems();
    renderControl.sync();
    renderControl.render();

    if(!fbo.toImage().save(destination))
    {
        glContext->doneCurrent();

        msg_error("AteosApplication::savePatientInfoInImage") << "Image could not be saved, possible reasons are : item has an incorrect size or the image destination path is incorrect";
        return false;
    }

    glContext->doneCurrent();

    return true;
}

bool MainApplication::runPythonScript(const QString& script)
{
    return PythonEnvironment::runString(script.toStdString());
}

bool MainApplication::runPythonFile(const QString& filename)
{
    return PythonEnvironment::runFile(filename.toLatin1().constData());
}

QVariantList MainApplication::executeProcess(const QString& command, int timeOutMsecs)
{
    QProcess process;
    process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process.setProcessChannelMode(QProcess::ForwardedChannels); // to display stdout/sterr (QProcess::MergedChannels for stdout only)

    process.start(command);

    process.waitForFinished(timeOutMsecs);

    return QVariantList() << QVariant::fromValue((int) process.exitStatus()) << QVariant::fromValue(process.exitCode()) << QVariant::fromValue(process.readAllStandardOutput()) << QVariant::fromValue(process.readAllStandardError());
}

ProcessState* MainApplication::executeProcessAsync(const QString& command)
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

void MainApplication::addNextFrameAction(QJSValue& jsFunction)
{
    if(!jsFunction.isCallable())
    {
        qWarning() << "SofaApplication::addNextFrameAction - parameter must be a JS function";
        return;
    }

    QTimer::singleShot(0, [&]() { jsFunction.call(); });
}

QString MainApplication::pythonDirectory() const
{
    return myPythonDirectory;
}

void MainApplication::setPythonDirectory(const QString& pythonDirectory)
{
    myPythonDirectory = pythonDirectory;
}

QString MainApplication::dataDirectory() const
{
    return myDataDirectory;
}

void MainApplication::setDataDirectory(const QString& dataDirectory)
{
    myDataDirectory = dataDirectory;

    if(!myDataDirectory.isEmpty() && !(myDataDirectory.endsWith("/") || myDataDirectory.endsWith("\\")))
        myDataDirectory += "/";
}

QString MainApplication::binaryDirectory() const
{
    return QCoreApplication::applicationDirPath() + "/";
}

void MainApplication::saveScreenshot(const QString& path)
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

QObject* MainApplication::instanciateURL(const QUrl& url)
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

int MainApplication::overrideCursorShape() const
{
    return QApplication::overrideCursor() ? QApplication::overrideCursor()->shape() : Qt::ArrowCursor;
}

void MainApplication::setOverrideCursorShape(int newCursorShape)
{
    if(newCursorShape == overrideCursorShape())
        return;

    QApplication::restoreOverrideCursor();
    if(Qt::ArrowCursor != newCursorShape)
        QApplication::setOverrideCursor(QCursor((Qt::CursorShape) newCursorShape));

    overrideCursorShapeChanged();
}

QPointF MainApplication::mapItemToScene(QQuickItem* item, const QPointF& point) const
{
    if(!item)
        return QPointF();

    return item->mapToScene(point);
}

bool MainApplication::isAncestorItem(QQuickItem* item, QQuickItem* ancestorItem) const
{
    if(item && ancestorItem && item != ancestorItem)
        for(QQuickItem* parentItem = item->parentItem(); parentItem; parentItem = parentItem->parentItem())
            if(ancestorItem == parentItem)
                return true;

    return false;
}

QQuickWindow* MainApplication::itemWindow(QQuickItem* item) const
{
    if(!item)
        return nullptr;

    return item->window();
}

void MainApplication::trimComponentCache(QObject* object)
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

void MainApplication::clearSettingGroup(const QString& group)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.remove("");
    settings.endGroup();
}

int MainApplication::objectDepthFromRoot(QObject* object)
{
    int depth = -1;

    if(object)
        for(QObject* ancestor = object->parent(); ancestor; ancestor = ancestor->parent())
            ++depth;

    return depth;
}

QString MainApplication::toLocalFile(const QUrl& url)
{
    return url.toLocalFile();
}

QQuaternion MainApplication::quaternionFromEulerAngles(const QVector3D& eulerAngles) const
{
    return QQuaternion::fromEulerAngles(eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
}

QVector3D MainApplication::quaternionToEulerAngles(const QQuaternion& quaternion) const
{
    return quaternion.toEulerAngles();
}

QVariantList MainApplication::quaternionToAxisAngle(const QQuaternion& quaternion) const // return [QVector3D axis, float angle];
{
    QVector3D axis;
    float angle = 0.0f;

    quaternion.getAxisAndAngle(&axis, &angle);

    return QVariantList() << QVariant::fromValue(axis) << QVariant::fromValue(angle);
}

QQuaternion MainApplication::quaternionFromAxisAngle(const QVector3D& axis, float angle) const
{
    return QQuaternion::fromAxisAndAngle(axis, angle);
}

QQuaternion MainApplication::quaternionDifference(const QQuaternion& q0, const QQuaternion& q1) const
{
    return q1 * q0.conjugated();
}

QQuaternion MainApplication::quaternionMultiply(const QQuaternion& q0, const QQuaternion& q1) const
{
    return q0 * q1;
}

QQuaternion MainApplication::quaternionConjugate(const QQuaternion& q) const
{
    return q.conjugated();
}

QVector3D MainApplication::quaternionRotate(const QQuaternion& q, const QVector3D& v) const
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
QVariantList MainApplication::quaternionSwingTwistDecomposition(const QQuaternion& q, const QVector3D& direction)
{
    QVector3D ra(q.x(), q.y(), q.z()); // rotation axis
    QVector3D p = direction.normalized() * QVector3D::dotProduct(ra, direction.normalized()); // projection( ra, direction ); // return projection v1 on to v2  (parallel component)

    QQuaternion twist(q.scalar(), p.x(), p.y(), p.z());
    twist.normalize();

    QQuaternion swing = q * twist.conjugated();

    return QVariantList() << QVariant::fromValue(swing) << QVariant::fromValue(twist);
}

void MainApplication::SetOpenGLDebugContext()
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->setOpenGLDebugContext();
}

void MainApplication::UseOpenGLDebugLogger()
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->useOpenGLDebugLogger();
}

void MainApplication::UseDefaultSofaPath()
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->useDefaultSofaPath();
}

void MainApplication::ApplySettings(const QString& defaultSettingsPath, const QString& backupSettingsPath)
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->applySettings(defaultSettingsPath, backupSettingsPath);
}

void MainApplication::FinalizeSettings()
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->finalizeSettings();
}


void MainApplication::MakeBackupSettings(const QString& backupSettingsPath)
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->makeBackupSettings(backupSettingsPath);
}

void MainApplication::ApplyBackupSettings(const QString& backupSettingsPath)
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->applyBackupSettings(backupSettingsPath);
}

void MainApplication::ApplyDefaultSettings(const QString& defaultSettingsPath, const QString& backupSettingsPath)
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->applyDefaultSettings(defaultSettingsPath,backupSettingsPath);
}

void MainApplication::CopySettings(const QSettings& src, QSettings& dst)
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->copySettings(src,dst);
}

bool MainApplication::Initialization()
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return false;
    }
    return m_maininstance->surfaceDefinition() ;
}

void MainApplication::Instanciate(QQmlApplicationEngine& applicationEngine)
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->instanciate(applicationEngine);
}

void MainApplication::Destruction()
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return ;
    }
    m_maininstance->destruction();
}

bool MainApplication::MainInitialization(QApplication& app, QQmlApplicationEngine &applicationEngine, const QString& mainScript)
{
    if(!m_maininstance){
        dmsg_error("SofaApplication") << "m_maininstance is not set. Abort."  ;
        return false;
    }
    return m_maininstance->mainInitialization(app, applicationEngine, mainScript);
}

void MainApplication::setApplicationSingleton(SofaApplication *app)
{
    m_maininstance = app ;
}

SofaApplication* MainApplication::getApplicationSingleton()
{
    return m_maininstance ;
}


}

}
