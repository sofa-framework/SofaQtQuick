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

#include <GL/glew.h>

#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaQtQuickGUI/SofaBaseScene.h>
#include <SofaQtQuickGUI/SofaProject.h>
#include <SofaQtQuickGUI/ProcessState.h>

#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/Utils.h>
#include <sofa/helper/BackTrace.h>
#include <sofa/helper/system/console.h>
#include <sofa/helper/logging/Messaging.h>
#include <sofa/helper/logging/ConsoleMessageHandler.h>
#include <sofa/helper/AdvancedTimer.h>

#include <SofaPython3/PythonEnvironment.h>
using sofapython3::PythonEnvironment;

#include "SofaQtQuick_PythonEnvironment.h"

#include <QQuickStyle>
#include <QInputDialog>
#include <QLineEdit>
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
#include <QOffscreenSurface>
#include <QRunnable>
#include <QTimer>
#include <QDirIterator>
#include <QFileDialog>

#include <SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h>
using sofaqtquick::bindings::SofaCoreBindingContext;

#include <SofaQtQuickGUI/Modules/SofaRuntime/SofaRuntimeModule.h>
using sofaqtquick::SofaRuntimeModule;

#include <SofaQtQuickGUI/SofaQtQuickQmlModule.h>
using sofaqtquick::SofaQtQuickQmlModule;

#include <QQmlDebuggingEnabler>
QQmlDebuggingEnabler enabler;

#include <SofaQtQuickGUI/Bindings/SofaData.h>

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <signal.h>
#endif

namespace sofaqtquick
{

using namespace sofa::helper;
using namespace sofa::simulation;

SofaBaseApplication* SofaBaseApplication::OurInstance = nullptr;

SofaBaseApplication::SofaBaseApplication(QObject* parent) : QObject(parent),
    myPythonDirectory(),
    myDataDirectory()
{
    OurInstance = this;

    // look for the data directory next to the executable location
    if(QCoreApplication::applicationName().isEmpty())
        qCritical() << "The QCoreApplication::applicationName should have been setted before SofaBaseApplication instantiation";

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

    connect(&m_viewUpdater, &QTimer::timeout, [](){
        SofaBaseApplication::updateAllDataView();
    });
    m_viewUpdater.start(70);

    /// Initialize the general python3 environment.
    sofapython3::PythonEnvironment::Init();

    /// Initialize the layer specific to sofaqtquick environment.
    sofaqtquick::PythonEnvironment::Init();

    m_currentProject = new SofaProject();
}

SofaBaseApplication::~SofaBaseApplication()
{
    if(this == OurInstance)
        OurInstance = nullptr;

    sofapython3::PythonEnvironment::Release();
}

SofaBaseApplication* SofaBaseApplication::Instance()
{
    return OurInstance;
}

void SofaBaseApplication::copyToClipboard(const QString& text)
{
    QClipboard* clipboard = QApplication::clipboard();
    if(!clipboard)
        return;

    clipboard->setText(text);
}

void SofaBaseApplication::openInExplorer(const QString& folder) const
{
    if (QFileInfo(folder).isFile())
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(folder).path()));
    else
        QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
}

void SofaBaseApplication::openInTerminal(const QString& folder) const
{
    QSettings settings;
    if(!settings.contains("TerminalEmulator"))
    {
        settings.setValue("TerminalEmulator", "gnome-terminal"); // can set gnome-terminal for instance
    }
    if(!settings.contains("TerminalEmulatorParams"))
        settings.setValue("TerminalEmulatorParams", "--working-directory=${path}");

    QString program = settings.value("TerminalEmulator").toString();
    QString dir;
    if (QFileInfo(folder).isFile())
        dir = QFileInfo(folder).path();
    else
        dir = folder;
    QProcess::execute(program, QStringList() << settings.value("TerminalEmulatorParams").toString().replace("${path}", dir));
}

///
/// \brief SofaBaseApplication::openInEditor
/// Opens in the editor specified in 'DefaultEditor'
///
/// \param fullpath substituted by ${path} in the DefaultEditorParams
/// \param lineno substituted by ${lineno} in the DefaultEditorParams
///
void SofaBaseApplication::openInEditor(const QString& fullpath, const int lineno) const
{
    QSettings settings;
    if(!settings.contains("DefaultEditor"))
    {
        QProcessEnvironment env;
        settings.setValue("DefaultEditor", "qtcreator"); // can set emacs instead in runSofa2.ini for instance
    }
    if(!settings.contains("DefaultEditorParams"))
        settings.setValue("DefaultEditorParams", "-client ${path}:${lineno}"); // ex. for qtcreator: "-client ${path}:${lineno}"


    QString path = QFileInfo(fullpath).absoluteFilePath();

    QFileInfo finfo(path);
    if (!finfo.exists()) {
        QFile file(path);
        file.open(QIODevice::NewOnly);
        if (file.isOpen()) file.close();
    }
    QString line = std::to_string(lineno).c_str();

    QString editor = settings.value("DefaultEditor").toString();
    QStringList args = settings.value("DefaultEditorParams").toString().replace("${path}", path).replace("${lineno}", line).split(" ");
    QProcess process;
    int ret = process.startDetached(editor, args);
    if (ret < 0)
    {
        msg_warning("OpenInEditor") << "Failed to launch chosen editor. Check runSofa2.ini";
        QDesktopServices::openUrl(QUrl::fromLocalFile(fullpath));
    }
}

///
/// \brief SofaBaseApplication::openInEditorFromUrl
/// Opens the file pointed in the url with the editor specified in 'DefaultEditor'
///
/// \param fullpath substituted by ${path} in the DefaultEditorParams
/// \param lineno substituted by ${lineno} in the DefaultEditorParams
///
void SofaBaseApplication::openInEditorFromUrl(const QUrl& fullpath, const int lineno) const
{
    openInEditor(fullpath.path(), lineno);
}

QString SofaBaseApplication::createFolderIn(const QString& parent)
{
    QString path = parent;
    QFileInfo finfo(parent);

    if (!finfo.isDir())
        path = finfo.path();
    bool ok;
    const QString dirName = QInputDialog::getText(nullptr,
                                                  tr("Type in the new folder's name:"),
                                                  tr("Folder Name: "),
                                                  QLineEdit::Normal,
                                                  "New Folder", &ok);

    QDir dir(path+ "/" + dirName);
    if (createFolder(dir.path()))
        return "";
    return dir.path();
}

bool SofaBaseApplication::createFolder(const QString& destination)
{
    QDir dir(destination);
    if (!dir.exists())
        return dir.mkpath(".");

    return false;
}

bool SofaBaseApplication::removeFolder(const QString& destination)
{
    QDir dir(destination);
    return dir.removeRecursively();
}

bool SofaBaseApplication::copyFolder(const QString& source, const QString& destination)
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

QStringList SofaBaseApplication::findFiles(const QString& dirPath, const QStringList& nameFilters)
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

QString SofaBaseApplication::loadFile(const QString& filename)
{
    QFile file(filename);
    if(file.open(QFile::ReadOnly))
        return QString(file.readAll());

    return QString();
}

bool SofaBaseApplication::saveFile(const QString& destination, const QString& data)
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

bool SofaBaseApplication::copyFile(const QString& source, const QString& destination)
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

bool SofaBaseApplication::fileExists(const QString &filepath)
{
    QFileInfo finfo(filepath);
    return finfo.exists();
}

QImage SofaBaseApplication::screenshotComponent(QQuickItem* item, const QSize& forceSize) const
{
    if(!item)
    {
        msg_error("SofaBaseApplication::screenshotComponent") << "QQuickItem is null";
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

QImage SofaBaseApplication::screenshotComponent(QQmlComponent* component, const QSize& forceSize) const
{
    if(!component)
    {
        msg_error("SofaBaseApplication::screenshotComponent") << "QQmlComponent is null";
        return QImage();
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(component->create());

    return screenshotComponent(item, forceSize);
}

QImage SofaBaseApplication::screenshotComponent(const QUrl& componentUrl, const QSize& forceSize) const
{
    QQmlEngine* engine = qmlEngine(this);

    QQmlComponent component(engine, componentUrl);
    if(component.status() != QQmlComponent::Ready)
    {
        msg_error("SofaBaseApplication::screenshotComponent") << "QQmlComponent is not ready, errors are:" << component.errorString().toStdString();
        return QImage();
    }

    return screenshotComponent(&component, forceSize);
}

bool SofaBaseApplication::screenshotComponent(QQuickItem* item, const QString& destination, const QSize& forceSize) const
{
    QImage image = screenshotComponent(item, forceSize);

    if(!image.save(destination))
    {
        msg_error("SofaBaseApplication::screenshotComponent") << "Image could not be saved, possible reasons are : item has an incorrect size or the image destination path is incorrect";
        return false;
    }

    return true;
}

bool SofaBaseApplication::screenshotComponent(QQmlComponent* component, const QString& destination, const QSize& forceSize) const
{
    if(!component)
    {
        msg_error("SofaBaseApplication::screenshotComponent") << "QQmlComponent is null";
        return false;
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(component->create());

    return screenshotComponent(item, destination, forceSize);
}

bool SofaBaseApplication::screenshotComponent(const QUrl& componentUrl, const QString& destination, const QSize& forceSize) const
{
    QQmlEngine* engine = qmlEngine(this);

    QQmlComponent component(engine, componentUrl);
    if(component.status() != QQmlComponent::Ready)
    {
        msg_error("SofaBaseApplication::screenshotComponent") << "QQmlComponent is not ready, errors are:" << component.errorString().toStdString();
        return false;
    }

    return screenshotComponent(&component, destination, forceSize);
}

bool SofaBaseApplication::runPythonScript(const QString& script)
{
    return sofapython3::PythonEnvironment::runString(script.toStdString());
}

bool SofaBaseApplication::runPythonFile(const QString& filename)
{
    return sofapython3::PythonEnvironment::runFile(filename.toLatin1().constData());
}

QVariantList SofaBaseApplication::executeProcess(const QString& command, int timeOutMsecs)
{
    QProcess process;
    process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process.setProcessChannelMode(QProcess::ForwardedChannels); // to display stdout/sterr (QProcess::MergedChannels for stdout only)

    process.start(command);

    process.waitForFinished(timeOutMsecs);

    return QVariantList() << QVariant::fromValue((int) process.exitStatus()) << QVariant::fromValue(process.exitCode()) << QVariant::fromValue(process.readAllStandardOutput()) << QVariant::fromValue(process.readAllStandardError());
}


ProcessState* SofaBaseApplication::executeProcessAsync(const QString& command, const QStringList& arguments, const QString& workingDirectory)
{
    QProcess* process = new QProcess();

    QUrl url = QUrl::fromUserInput(workingDirectory);
    if(workingDirectory.size()!=0)
    {
        process->setWorkingDirectory(url.toLocalFile());
    }

    process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process->setProcessChannelMode(QProcess::MergedChannels); // to display stdout/sterr (QProcess::MergedChannels for stdout only)

    ProcessState* processState = new ProcessState(process);

    process->startDetached(command, arguments, url.path());
    process->waitForFinished();

    QByteArray result = process->readAll();

    return processState;

    // do not forget to call destroyProcess to delete "process"
    // how to delete "processState"?
}

void SofaBaseApplication::addNextFrameAction(QJSValue& jsFunction)
{
    if(!jsFunction.isCallable())
    {
        qWarning() << "SofaBaseApplication::addNextFrameAction - parameter must be a JS function";
        return;
    }

    QTimer::singleShot(0, [&]() { jsFunction.call(); });
}

QString SofaBaseApplication::pythonDirectory() const
{
    return myPythonDirectory;
}

void SofaBaseApplication::setPythonDirectory(const QString& pythonDirectory)
{
    myPythonDirectory = pythonDirectory;
}

QString SofaBaseApplication::dataDirectory() const
{
    return myDataDirectory;
}

void SofaBaseApplication::setDataDirectory(const QString& dataDirectory)
{
    myDataDirectory = dataDirectory;

    if(!myDataDirectory.isEmpty() && !(myDataDirectory.endsWith("/") || myDataDirectory.endsWith("\\")))
        myDataDirectory += "/";
}

QString SofaBaseApplication::binaryDirectory() const
{
    return QCoreApplication::applicationDirPath() + "/";
}

QString SofaBaseApplication::templatesDirectory() const
{
    return QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/");
}

QString SofaBaseApplication::inspectorsDirectory() const
{
    QString templatesDir = templatesDirectory();
    QDir d(templatesDir);
    if (!d.exists("inspectors"))
        d.mkdir("inspectors");
    return templatesDir + "inspectors/";
}

QString SofaBaseApplication::assetsDirectory() const
{
    QString templatesDir = templatesDirectory();
    QDir d(templatesDir);
    if (!d.exists("assets")) d.mkdir("assets");
    return templatesDir + "assets/";
}

bool SofaBaseApplication::createInspector(QString file)
{
    if (!fileExists(file)) {
        QFile f(file);
        f.open(QIODevice::WriteOnly);
        if (f.isOpen()) {
            f.write("import QtQuick 2.0                              \n"
                       "import CustomInspectorWidgets 1.0               \n"
                       "                                                \n"
                       "CustomInspector {                               \n"
                       "    dataDict: {                                 \n"
                       "        \"Base\": [\"name\",\"componentState\"] \n"
                       "    }                                           \n"
                       "}                                               \n"
                       );
            f.close();
            return true;
        }
    }
    return false;
}



void SofaBaseApplication::saveScreenshot(const QString& path)
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

QObject* SofaBaseApplication::instanciateURL(const QUrl& url)
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

int SofaBaseApplication::overrideCursorShape() const
{
    return QApplication::overrideCursor() ? QApplication::overrideCursor()->shape() : Qt::ArrowCursor;
}


sofaqtquick::SofaProject* SofaBaseApplication::getCurrentProject()
{
    return m_currentProject;
}

void SofaBaseApplication::setCurrentProject(sofaqtquick::SofaProject* newProject)
{
    if (m_currentProject)
        delete m_currentProject;
    m_currentProject = newProject;
}

void SofaBaseApplication::setProjectDirectory(const std::string& dir)
{
    if (m_currentProject)
    {
        std::string directory = sofa::helper::system::DataRepository.getFile(dir);
        QDir dir(directory.c_str());
        m_currentProject->setRootDir(QUrl(dir.absolutePath()));
    }
    else {
        msg_error("SofaQtQuickGUI") << "Cannot open project directory: SofaProject not instantiated";
    }
}
std::string SofaBaseApplication::getProjectDirectory()
{
    return m_currentProject->getRootDir().path().toStdString();
}


void SofaBaseApplication::setOverrideCursorShape(int newCursorShape)
{
    if(newCursorShape == overrideCursorShape())
        return;

    QApplication::restoreOverrideCursor();
    if(Qt::ArrowCursor != newCursorShape)
        QApplication::setOverrideCursor(QCursor((Qt::CursorShape) newCursorShape));

    overrideCursorShapeChanged();
}

QPointF SofaBaseApplication::mapItemToScene(QQuickItem* item, const QPointF& point) const
{
    if(!item)
        return QPointF();

    return item->mapToScene(point);
}

bool SofaBaseApplication::isAncestorItem(QQuickItem* item, QQuickItem* ancestorItem) const
{
    if(item && ancestorItem && item != ancestorItem)
        for(QQuickItem* parentItem = item->parentItem(); parentItem; parentItem = parentItem->parentItem())
            if(ancestorItem == parentItem)
                return true;

    return false;
}

QQuickWindow* SofaBaseApplication::itemWindow(QQuickItem* item) const
{
    if(!item)
        return nullptr;

    return item->window();
}

void SofaBaseApplication::trimComponentCache(QObject* object)
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

void SofaBaseApplication::clearSettingGroup(const QString& group)
{
    QSettings settings;
    settings.beginGroup(group);
    settings.remove("");
    settings.endGroup();
}

int SofaBaseApplication::objectDepthFromRoot(QObject* object)
{
    int depth = -1;

    if(object)
        for(QObject* ancestor = object->parent(); ancestor; ancestor = ancestor->parent())
            ++depth;

    return depth;
}

QString SofaBaseApplication::toLocalFile(const QUrl& url)
{
    return url.isLocalFile() ? url.toLocalFile() : url.path();
}

void SofaBaseApplication::SetOpenGLDebugContext()
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

void SofaBaseApplication::UseOpenGLDebugLogger()
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if(nullptr == ctx) {
        msg_error("SofaQtQuickGUI") << "SofaBaseApplication::initializeDebugLogger has been called without a valid opengl context made current";
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

void SofaBaseApplication::UseDefaultSofaPath()
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
    return "SofaBaseApplication/ExitedNormally";
}

static QString CorruptedKey()
{
    return "SofaBaseApplication/Corrupted";
}

void SofaBaseApplication::ApplySettings(const QString& defaultSettingsPath, const QString& backupSettingsPath)
{
    QSettings::setDefaultFormat(QSettings::IniFormat);

    connect(qApp, &QApplication::aboutToQuit, &SofaBaseApplication::FinalizeSettings); // maybe too early ?

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

void SofaBaseApplication::FinalizeSettings()
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

void SofaBaseApplication::MakeBackupSettings(const QString& backupSettingsPath)
{
    QSettings settings;

    QString finalBackupSettingsPath = backupSettingsPath;
    if(finalBackupSettingsPath.isEmpty())
        finalBackupSettingsPath = DefaultBackupSettingsPath();

    QSettings backupSettings(finalBackupSettingsPath, QSettings::IniFormat);
    backupSettings.clear();

    CopySettings(settings, backupSettings);
}

void SofaBaseApplication::ApplyBackupSettings(const QString& backupSettingsPath)
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

void SofaBaseApplication::ApplyDefaultSettings(const QString& defaultSettingsPath, const QString& backupSettingsPath)
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

void SofaBaseApplication::CopySettings(const QSettings& src, QSettings& dst)
{
    SettingsCopyGroupsHelper(src, dst);
}

bool SofaBaseApplication::Initialization()
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

void SofaBaseApplication::InitOpenGL()
{    
    QOffscreenSurface* offScreenSurface = new QOffscreenSurface();
    offScreenSurface->create();
    if(!QOpenGLContext::currentContext())
    {
        // create an opengl context
        QOpenGLContext* openglContext = new QOpenGLContext();

        // share its resources with the global context
        QOpenGLContext* sharedOpenglContext = QOpenGLContext::globalShareContext();
        if(sharedOpenglContext)
            openglContext->setShareContext(sharedOpenglContext);

        if(!openglContext->create())
            qFatal("Cannot create an OpenGL Context needed to init SofaScene");

        if(!offScreenSurface->isValid())
            qFatal("Cannot create an OpenGL Surface needed to init SofaScene");

        openglContext->makeCurrent(offScreenSurface);
    }

    GLenum err = glewInit();
    if(0 != err)
        msg_error("SofaQtQuickGUI") << "GLEW Initialization failed with error code:" << err;
}

void SofaBaseApplication::removePendingDataViewUpdate(QmlDDGNode *request)
{
    //auto t = ;
    auto it = Instance()->m_pendingUpdates.find(request);
    if(it != Instance()->m_pendingUpdates.end())
    {
        Instance()->m_pendingUpdates.erase(it);
    }
}

void SofaBaseApplication::requestDataViewUpdate(QmlDDGNode *request)
{
    Instance()->m_pendingUpdates[request] = request->m_basedata->getCounter();
}

void SofaBaseApplication::updateAllDataView()
{
    auto& d = Instance()->m_pendingUpdates;
    for(auto& kv : d)
    {
        kv.first->m_sofadata->valueChanged(QVariant());
    }
    Instance()->m_pendingUpdates.clear();
}

void SofaBaseApplication::Instanciate(QQmlApplicationEngine& applicationEngine)
{
    // this will instanciate a SofaBaseApplication QML instance
    applicationEngine.loadData("import QtQuick 2.0\n"
                               "import SofaBaseApplication 1.0\n"
                               "QtObject {\n"
                               "Component.onCompleted: {SofaBaseApplication.objectName;}\n"
                               "}");
}

void SofaBaseApplication::Destruction()
{
    qApp->quit();
}

class UseOpenGLDebugLoggerRunnable : public QRunnable
{
    void run()
    {
        SofaBaseApplication::UseOpenGLDebugLogger();
    }
};

sofaqtquick::bindings::SofaBase* SofaBaseApplication::getSelectedComponent() const
{
    if(m_selectedComponent.get() == nullptr)
        return nullptr;
    return new SofaBase(m_selectedComponent.get());
}


void SofaBaseApplication::setSelectedComponent(sofaqtquick::bindings::SofaBase* selectedComponent)
{
    if(selectedComponent == nullptr)
    {
        if(m_selectedComponent) {
            setSelectedComponent(new SofaBase(m_selectedComponent));
            return;
        }
        emit selectedComponentChanged(new SofaBase(m_selectedComponent));
        return;
    }

    if(selectedComponent->rawBase() == m_selectedComponent)
        return;
    m_selectedComponent = selectedComponent->rawBase();
    emit selectedComponentChanged(selectedComponent);
}

void SofaBaseApplication::SetSelectedComponent(sofaqtquick::bindings::SofaBase* selectedComponent)
{
    assert(OurInstance != nullptr);
    OurInstance->setSelectedComponent(selectedComponent);
}

sofaqtquick::bindings::SofaBase* SofaBaseApplication::GetSelectedComponent()
{
    assert(OurInstance != nullptr);
    return OurInstance->getSelectedComponent();
}

bool SofaBaseApplication::DefaultMain(QApplication& app, QQmlApplicationEngine &applicationEngine, const QString& mainScript)
{
    OurInstance = new SofaBaseApplication();

    // color console
    sofa::helper::console::setStatus(sofa::helper::console::Status::On);

    // TODO: this command disable the multithreaded render loop, currently we need this because our implementation of the sofa interface is not thread-safe
    // "threaded" vs "basic"
    qputenv("QSG_RENDER_LOOP", "basic");

    if(!app.testAttribute(Qt::AA_ShareOpenGLContexts))
        qCritical() << "CRITICAL: SofaBaseApplication::initialization() must be called before QApplication instanciation";

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    // quit on Ctrl-C
    signal(SIGINT, [](int) {SofaBaseApplication::Destruction();});
#endif

    /// Set the application engine to use for the SofaCore binding.
    /// This is needed to generate JS exception from the c++ code.
    /// If you found an alternative design to avoid the singleton it is
    /// more than welcome.
    SofaCoreBindingContext::setQQmlEngine(&applicationEngine);

    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    app.addLibraryPath(QCoreApplication::applicationDirPath() + "/../lib/");
    
    // initialise paths
    SofaBaseApplication::UseDefaultSofaPath();

    // add application data path
    sofa::helper::system::DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/" + app.applicationName() + "Data").toStdString());
    sofa::helper::system::DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/../data").toStdString());
    sofa::helper::system::DataRepository.addFirstPath((QCoreApplication::applicationDirPath() + "/../" + app.applicationName() + "Data").toStdString());

    /// Register the Sofa Binding into the QML Types system.
    SofaQtQuickQmlModule::RegisterTypes(&applicationEngine);
    SofaRuntimeModule::RegisterTypes();

    // compute command line arguments
    QCommandLineParser parser;
    QCommandLineOption sceneOption(QStringList() << "s" << "scene", "Load with this scene", "file");
    QCommandLineOption projectOption(QStringList() << "p" << "project", "Load with this project", "directory");
    QCommandLineOption guiConfigOption(QStringList() << "gc" << "guiconfig", "Load with this GUI configuration (absolute path to an ini file OR a config name)", "guiconfig");
    QCommandLineOption animateOption(QStringList() << "a" << "animate", "Start in animate mode");
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", "Fullscreen mode");
    QCommandLineOption widthOption(QStringList() << "width", "Window width", "pixels");
    QCommandLineOption heightOption(QStringList() << "height", "Window height", "pixels");
    QCommandLineOption logTimeOption(QStringList() << "log", "Log time during simulation");
    
    parser.addOption(sceneOption);
    parser.addOption(projectOption);
    parser.addOption(guiConfigOption);
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

    SofaBaseApplication::ApplySettings();

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

        if (parser.isSet(projectOption))
            OurInstance->getCurrentProject()->setRootDir(parser.value(projectOption));
        if(parser.isSet(animateOption) || parser.isSet(sceneOption))
        {
            SofaBaseScene* sofaScene = object->findChild<SofaBaseScene*>();
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
