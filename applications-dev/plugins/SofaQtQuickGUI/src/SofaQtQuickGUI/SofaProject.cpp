#include "SofaProject.h"
#include "SofaBaseApplication.h"

#include <sofa/helper/Utils.h>
#include <sofa/helper/system/FileMonitor.h>
using sofa::helper::system::FileEventListener;
using sofa::helper::system::FileMonitor;

#include "SofaQtQuickGUI/SofaBaseApplication.h"
#include "SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.h"
using RSPythonEnvironment = sofaqtquick::PythonEnvironment;

#include <SofaPython3/PythonEnvironment.h>
#include <SofaPython3/PythonFactory.h>
using sofapython3::PythonEnvironment;
namespace py = pybind11;

#include <SofaPython3/PythonFactory.h>
using sofapython3::PythonFactory;

#include <QWindow>
#include <QInputDialog>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QTimer>

#include "Assets/DirectoryAsset.h"
#include "Assets/AssetFactory.h"
using sofaqtquick::AssetFactory;

#include <fstream>

/// This is needed to implement filtering of msg_info.
namespace sofa::helper::logging
{

inline bool notMuted(const sofaqtquick::SofaProject* p){ return p->getDebug(); }
inline ComponentInfo::SPtr getComponentInfo(const sofaqtquick::SofaProject* )
{
    return ComponentInfo::SPtr(new ComponentInfo("SofaProject")) ;
}
}


namespace sofaqtquick {

ProjectMonitor::ProjectMonitor()
{
    connect(&m_dirwatcher, &QFileSystemWatcher::directoryChanged,
            this, &ProjectMonitor::directoryChanged);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(200);
}

void ProjectMonitor::update()
{
    FileMonitor::updates(0);
}

void ProjectMonitor::addDirectory(const QString& filepath)
{
    m_dirwatcher.addPath(filepath);
}

void ProjectMonitor::addFile(const QString& filepath)
{
    FileMonitor::addFile(filepath.toStdString(), this);
}

void ProjectMonitor::addPath(const QString& filepath)
{
    QFileInfo finfo {filepath};
    if(finfo.isFile())
        addFile(filepath);
    else
        addDirectory(filepath);
}

void ProjectMonitor::fileHasChanged(const std::string& filename){
    emit fileChanged(QString::fromStdString(filename));
}

void ProjectMonitor::removePath(const QString& filepath)
{
    QFileInfo finfo {filepath};
    if(finfo.isFile())
        FileMonitor::removeFileListener(finfo.absoluteFilePath().toStdString(), this);
    else
        m_dirwatcher.removePath(finfo.absoluteFilePath());
}

SofaProject::SofaProject()
{
    m_watcher = new ProjectMonitor();
    connect(m_watcher, &ProjectMonitor::directoryChanged, this, &SofaProject::onDirectoryChanged);
    connect(m_watcher, &ProjectMonitor::fileChanged, this, &SofaProject::onFileChanged);
}

SofaProject::~SofaProject()
{
    if(m_watcher!=nullptr)
        delete m_watcher;
}

void SofaProject::setRootDir(const QUrl& rootDir)
{
    if(rootDir.isEmpty())
        return;
    m_rootDir = rootDir;
    m_assets.clear();
    m_directories.clear();

    msg_info() << "Setting root directory to '" << rootDir.toString().toStdString()<<"'";
    QFileInfo root = QFileInfo(m_rootDir.path());
    scan(root);
}

const QUrl& SofaProject::getRootDir() { return m_rootDir;  }

bool SofaProject::getDebug() const { return m_debug; }
void SofaProject::setDebug(bool state) {
    m_debug = state;
    emit debugChanged(state);
}


void SofaProject::onDirectoryChanged(const QString &path)
{
    msg_info() << "onDirectoryChanged: " << path.toStdString() ;
    updateDirectory(QFileInfo(path));
}

void SofaProject::onFileChanged(const QString &path)
{
    msg_info() << "onFileChanged: " << path.toStdString();
    updateAsset(QFileInfo(path));
}

void SofaProject::updateDirectory(const QFileInfo& finfo)
{
    if (!finfo.exists())
        return;

    msg_info() << "updateDirectory: " << finfo.absoluteFilePath().toStdString() ;
    QString filePath = finfo.absoluteFilePath();

    std::map<QString, bool> stillThere;

    /// We add the children of the current directory.
    for (QFileInfo& nfinfo : QDir(finfo.filePath()).entryInfoList())
    {
        if(isFileExcluded(nfinfo))
            continue;


        if( nfinfo.isFile() )
        {
            /// Add the new content.
            if( !m_assets.contains(nfinfo.absoluteFilePath()) )
            {
                scan(nfinfo);
            }

            if( m_assets[nfinfo.absoluteFilePath()]->getLastModified()  != nfinfo.lastModified())
            {
                updateAsset(nfinfo);
            }
            continue;
        }

        /// Add the new content.
        if( !m_directories.contains(nfinfo.absoluteFilePath()) )
        {
            scan(nfinfo);
        }
    }

    removeDirEntries(*m_directories[filePath].get());

    /// Update the content of the directory cache.
    m_directories[filePath]->getDetails();
    return;
}

void SofaProject::removeDirEntries(DirectoryAsset& folder)
{
    /// Remove the old content that is now removed.
    /// traverse all the old entries and search if they still exists.
    /// If not remove them from the corresponding asset & directory storage.
    for(auto& cfinfo : folder.content)
    {
        if(isFileExcluded(cfinfo))
            continue;

        bool wasFile = cfinfo.isFile();
        cfinfo.refresh();

        /// If the file does not exists anymore we remove it from the caches.
        if(!cfinfo.exists())
        {
            /// Remove the file monitor.
            m_watcher->removePath(cfinfo.absoluteFilePath());

            /// Remove the entry in the asset/directory cache
            if(wasFile)
            {
                msg_info() << "removing: " << cfinfo.absoluteFilePath().toStdString();
                m_assets.remove(cfinfo.absoluteFilePath());
            }else
            {
                msg_info() << "removing: " << cfinfo.absoluteFilePath().toStdString();
                removeDirEntries(*m_directories[cfinfo.absoluteFilePath()].get());
                m_directories.remove(cfinfo.absoluteFilePath());
            }
        }
    }
}

bool SofaProject::isFileExcluded(const QFileInfo &finfo)
{
    return finfo.fileName() == "." || finfo.fileName() == ".." || finfo.fileName() == "__pycache__";
}

void SofaProject::scan(const QUrl& folder)
{
    scan(QFileInfo(folder.path()));
}

void SofaProject::scan(const QFileInfo& file)
{
    if (!file.exists())
        return;

    QString filePath = file.absoluteFilePath();
    if (m_assets.contains(filePath))
        return;

    if (m_directories.contains(filePath))
        return;

    msg_info() << "starting monitoring: " << file.absoluteFilePath().toStdString() ;
    m_watcher->addPath(file.absoluteFilePath());

    /// The file needs to be added
    if(file.isFile())
    {
        msg_info() << "register asset: " << file.absoluteFilePath().toStdString() ;
        m_assets[filePath] = AssetFactory::createInstance(file.filePath(), file.suffix());
        return;
    }

    msg_info() << "register dir: " <<  file.absoluteFilePath().toStdString() ;
    std::shared_ptr<DirectoryAsset> dirAsset { new DirectoryAsset(file.filePath()) };
    dirAsset->getDetails();
    m_directories[filePath] = dirAsset;

    /// We add the children of the current directory.
    for (QFileInfo& finfo : QDir(file.filePath()).entryInfoList())
    {
        if(isFileExcluded(finfo))
            continue;

        scan(finfo);
    }
    return;
}

QString readScriptTemplate(QString name, QString file) {
    QFile f(file);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        msg_error("SofaProject") << "file `" + file.toStdString()+"` does not exist";
        return "";
    }
    QTextStream in(&f);
    QString s = in.readAll();
    f.close();
    return s.replace("%ComponentName%", name);
}

QString SofaProject::createProject(const QUrl& dir)
{
    msg_error_when(createProjectTree(dir), "SofaProject::createProject()")
            << "Could not create directory tree for the new project";

    QString fileName = dir.path() + "/scenes/" + QFileInfo(dir.path()).baseName() + ".py";
    QString scriptContent = readScriptTemplate(QFileInfo(dir.path()).baseName(), QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyScene.py"));
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << scriptContent;

        file.close();
    }
    return fileName;
}

bool SofaProject::createProjectTree(const QUrl& dir)
{
    QDir d(dir.path());
    if (!d.exists())
        d.mkpath(".");

    bool ret = d.mkdir("assets");
    ret = ret & d.mkdir("assets");
    ret = ret & d.mkdir("scenes");
    ret = ret & d.mkdir("assets/resources");
    ret = ret & d.mkdir("assets/scripts");
    return ret;
}

QString SofaProject::importProject(const QUrl& srcUrl)
{
    QString src = srcUrl.path();
    QFileInfo finfo(src);
    if (finfo.exists() && finfo.suffix() == "zip")
    {
        QFileDialog dialog(nullptr, tr("Choose Project Destination"), "~/Documents", tr("All folders (*)"));
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly);
        if (dialog.exec())
        {
            QList<QUrl> folders = dialog.selectedUrls();
            if (folders.empty())
                return "Error: no destination picked";
            QApplication::setOverrideCursor(Qt::WaitCursor);
            QApplication::processEvents();
            QString dest = folders.first().path();
            QProcess process;
            process.start("/usr/bin/unzip", QStringList() << src << "-d" << dest);
            process.waitForFinished(-1);
            QApplication::restoreOverrideCursor();

            return dest + "/" + finfo.baseName();
        }
        return "Error: could not open Dialog";
    }
    return "Error: does not exist or isn't zip file";
}

bool SofaProject::exportProject(const QUrl& srcUrl)
{
    QString src = srcUrl.toLocalFile();
    QFileDialog dialog(nullptr, tr("Choose Project Destination"), "~/Documents");
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec())
    {
        QList<QUrl> folders = dialog.selectedUrls();
        if (folders.empty())
            return false;
        QString dest = folders.first().path();
        QProcess process;

        QApplication::setOverrideCursor(Qt::WaitCursor);
        QApplication::processEvents();
        QString filePath = QFileInfo(src).filePath();
        QString baseName = QFileInfo(src).baseName();
        QString fileName = QFileInfo(src).fileName();
        process.start("/bin/ln", QStringList() << "-s" << filePath << fileName);
        process.waitForFinished(-1);
        process.start("/usr/bin/zip", QStringList() << "-r" << QString(dest + "/" + baseName + ".zip") << fileName);
        process.waitForFinished(-1);
        process.start("/bin/rm", QStringList() << fileName);
        process.waitForFinished(-1);
        QApplication::restoreOverrideCursor();
        return true;
    }
    return false;
}

void SofaProject::updateAsset(const QFileInfo& file)
{
    QString filePath = file.absoluteFilePath();
    msg_info() << "updateAsset: " << filePath.toStdString();
    if(!m_assets.contains(filePath))
        m_assets[filePath] = AssetFactory::createInstance(file.filePath(), file.suffix());
    else
        m_assets[filePath]->getDetails();

    m_assets[filePath]->setLastModified(file.lastModified());
    emit filesChanged();
}

const QString SofaProject::getFileCount(const QUrl& url)
{
    QDir dir(url.path());
    if (!dir.exists())
        return "Corrupted Folder";
    std::string str = std::to_string(dir.count());
    str += " items";
    return QString(str.c_str());
}

Asset* SofaProject::getAsset(const QString& filePath)
{
    const auto& it = m_assets.find(filePath);
    if (it != m_assets.end())
    {
        msg_info() << "getAsset for: " << filePath.toStdString() << " => " << it.value();
        if(it.value() != nullptr)
            return it.value().get();
    }
    msg_info() << "getAsset not asset for: " << filePath.toStdString() ;

    return nullptr;
}

QStringList SofaProject::getSupportedTypes() const
{
    return AssetFactory::getSupportedTypes();
}

bool SofaProject::createPrefab(SofaBase* node)
{
    QFileDialog dialog(nullptr, tr("Save as Prefab"), this->getRootDir().toString(), tr("All files (*)"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    bool ret;
    QString name = QInputDialog::getText(nullptr, "Prefab Name:", "Name: ", QLineEdit::Normal, node->getName(), &ret);
    if (!ret)
        return false;
    QString help = QInputDialog::getText(nullptr, "Prefab Description:", "Description: ", QLineEdit::Normal, "", &ret);
    if (!ret)
        return false;
    if (dialog.exec())
    {
        std::string fileName = dialog.selectedFiles().first().toStdString();
        {
            sofapython3::PythonEnvironment::gil acquire;
            py::module::import("Sofa.Core");
            py::object rootNode = sofapython3::PythonFactory::toPython(node->base()->toBaseNode());
            py::module m = py::module::import("SofaQtQuick");
            return py::cast<bool>(m.attr("createPrefabFromNode")(fileName, rootNode, name.toStdString(), help.toStdString()));
        }
    }
    return false;
}


QString SofaProject::createTemplateFile(const QString& directory, const QString& templateType)
{
    QString scriptContent = "";
    QString extension = "py";
    if (templateType == "Canvas")
    {
        extension ="qml";
        scriptContent = readScriptTemplate("EmptyCanvas", QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyCanvas.qml"));
    }
    else if (templateType == "Controller")
        scriptContent = readScriptTemplate("EmptyController", QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyController.py"));
    else if (templateType == "DataEngine")
        scriptContent = readScriptTemplate("EmptyDataEngine", QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyDataEngine.py"));
    else if (templateType == "ForceField")
        scriptContent = readScriptTemplate("EmptyForceField", QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyForceField.py"));
    else if (templateType == "Prefab")
        scriptContent = readScriptTemplate("EmptyPrefab", QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyPrefab.py"));
    else if (templateType == "Scene")
        scriptContent = readScriptTemplate("EmptyScene", QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyScene.py"));

    QString dir;
    QFileInfo f(directory);
    dir = (f.isDir()) ? directory : f.dir().path();
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    QString strfile = dialog.getSaveFileName(nullptr, "Create New File", dir, "QtQuick UI (*."+extension+")");
    QFile file(strfile);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << scriptContent << endl;

        file.close();
        return strfile;
    }
    else {
        msg_error("SofaProject") << "Could not open file " << strfile.toStdString();
    }
    return QString();
}

bool SofaProject::createPythonPrefab(QString name, SofaBase* node)
{
    sofapython3::PythonEnvironment::gil acquire;

    // Maybe not the most pertinent method name...
    QString scriptContent = "";
    if (name.endsWith("Controller"))
        scriptContent = readScriptTemplate(name, QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyController.py"));
    else if (name.endsWith("DataEngine"))
        scriptContent = readScriptTemplate(name, QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyDataEngine.py"));
    else if (name.endsWith("ForceField"))
        scriptContent = readScriptTemplate(name, QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/emptyForceField.py"));
    else
        return false;

    if (scriptContent == "")
        return false;
    QString filepath = getRootDir().toLocalFile() + "/assets/scripts/" + name.toLower() + ".py";
    QFile f(filepath);
    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&f);
        stream << scriptContent << endl;
        f.close();
        py::list args;
        args.append(name.toUtf8().data());
        py::object instance = RSPythonEnvironment::CallFunction(filepath,
                                          name,
                                          args,
                                          py::dict(),
                                          node->rawBase());

        py::object n = PythonFactory::toPython(static_cast<sofa::simulation::graph::DAGNode*>(node->rawBase()->toBaseNode()));
        n.attr("addObject")(instance);
        SofaBaseApplication::Instance()->openInEditor(filepath);
        return true;
    }
    msg_error("SofaProject") << "could not open " << filepath.toStdString() << " in write-only.";
    return false;
}

void SofaProject::saveScene(const QString filepath, SofaNode* node)
{
    sofapython3::PythonEnvironment::executePython([this, filepath, node]()
    {
        std::string ppath = filepath.toStdString();
        py::module SofaQtQuick = py::module::import("SofaQtQuick");
        SofaQtQuick.reload();

        py::object rootNode = PythonFactory::toPython(node->self());

        py::str file(ppath);
        bool ret =  py::cast<bool>(SofaQtQuick.attr("saveAsPythonScene")(file, rootNode));
        if (ret) {
            msg_info("runSofa2") << "File saved to "  << ppath;
        } else {
            msg_error("runSofa2") << "Could not save to file "  << ppath;
        }
    });
    return;
}


}  /// namespace sofaqtquick
