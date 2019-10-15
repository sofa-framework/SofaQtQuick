#include "SofaProject.h"
#include "SofaApplication.h"

#include <sofa/helper/system/FileMonitor.h>
using sofa::helper::system::FileMonitor;

#include <SofaPython3/PythonEnvironment.h>
#include <SofaPython3/PythonFactory.h>
using sofapython3::PythonEnvironment;
namespace py = pybind11;
#include <QWindow>
#include <QInputDialog>
#include <QFileDialog>
#include "Assets/AssetFactory.h"
using sofa::qtquick::AssetFactory;

#include <fstream>

namespace sofa {
namespace qtquick {

class ProjectFileMonitor : public FileEventListener
{
public:
    ProjectFileMonitor(SofaProject* project)
    {
        m_project=project;
    }
    void fileHasChanged(const std::string& filename)
    {
        msg_info("ProjectFileMonitor") << "Update: " << filename;
        m_project->updatePath(filename);
    }
    SofaProject* m_project;
};

SofaProject::SofaProject()
{
    m_fileMonitor = new ProjectFileMonitor(this);
}

SofaProject::~SofaProject() {
    if (m_fileMonitor)
        delete m_fileMonitor;
}

const QUrl& SofaProject::getRootDir() { return m_rootDir;  }

void SofaProject::setRootDir(const QUrl& rootDir)
{
    m_rootDir = rootDir;
    if(rootDir.isEmpty())
    {
        msg_warning("SofaProject") << "Empty project directory...skipping." ;
        return;
    }

    msg_info("SofaProject") << "Setting root directory to '" << rootDir.toString().toStdString()<<"'";

    QFileInfo root = QFileInfo(m_rootDir.path());
    scanProject(root);
}


//void LiveFileMonitor::addPathToMonitor(const std::string& path)
//{
//    std::string p = path;
//    if (p.back() == '/')
//        p.pop_back();
//    std::vector<std::string> files;
//    FileSystem::listDirectory(p, files) ;
//    for(auto& filename: files)
//    {
//        if (FileSystem::isDirectory(p + "/" + filename))
//        {
//            addPathToMonitor(p + "/" + filename) ;
//        }
//        else
//        {
//            if (FileSystem::isFile(p + "/" + filename))
//            {
//                sofa::helper::system::FileMonitor::addFile(p + "/" + filename, this);
//            }
//        }
//    }
//}

void SofaProject::onFilesChanged()
{
    //emit filesChanged();
}

QString SofaProject::createProject(const QUrl& dir)
{
    msg_error_when(createProjectTree(dir), "SofaProject::createProject()")
            << "Could not create directory tree for the new project";

    QString fileName = dir.path() + "/scenes/" + QFileInfo(dir.path()).baseName() + ".pyscn";
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << "def createScene(root):" << endl
               << "    return root" << endl;

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

void SofaProject::updatePath(const std::string& filepath)
{

    QFileInfo finfo { QString::fromStdString(filepath) };
    scanProject(finfo);
}

void SofaProject::scanProject(const QUrl& folder)
{
    _scanProject(QFileInfo(folder.path()));
}


void SofaProject::scanProject(const QFileInfo& folder)
{
    //QApplication::setOverrideCursor(Qt::WaitCursor);
    //QApplication::processEvents();


    _scanProject(folder);

    //QApplication::restoreOverrideCursor();
}

void SofaProject::_scanProject(const QFileInfo& file)
{
    if (!file.exists())
        return;

    msg_info("SofaProject") << file.absoluteFilePath().toStdString() ;

    QString filePath = file.absoluteFilePath();

    /// Already monitored...
    if (m_assets.find(file.absoluteFilePath()) == m_assets.end())
    {
        msg_info("SofaProject") << file.absoluteFilePath().toStdString() << " REGISTERING THE FILE" ;
        FileMonitor::addFile(filePath.toStdString(), m_fileMonitor);
    }

    /// The file needs to be added
    if(file.isFile())
    {
        msg_info("SofaProject") << file.absoluteFilePath().toStdString() << " ADDING FILE IN REPOSITORY !" ;
        m_assets[filePath] = AssetFactory::createInstance(file.filePath(), file.suffix());
        return;
    }

    msg_info("SofaProject") << file.absoluteFilePath().toStdString() << " ADDING DIR IN REPOSITORY !" ;
    m_assets[filePath] = AssetFactory::createInstance(file.filePath(), "DIRECTORY");

    /// We add the children of the current directory.
    for (QFileInfo& finfo : file.dir().entryInfoList())
    {
        _scanProject(finfo);
    }
    return;

//    QStringList content = folder.entryList();
//    QFileInfoList contentInfo = folder.entryInfoList();
//    for (int idx = 0 ; idx < content.size() ; ++idx)
//    {
//        QFileInfo f = contentInfo[idx];
//        if (f.isDir())
//        {
//            if (f.fileName() == "." || f.fileName() == "..")
//                continue;
//            //_scanProject(QDir(f.filePath()));
//        }
//        else
//        {
//            m_assets.insert(assetMapPair(f.filePath(),
//                                         AssetFactory::createInstance(
//                                             f.filePath(), f.suffix())));
//        }
//    }
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
    if (it != m_assets.end() && it->second != nullptr)
        return it->second.get();
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
            py::module::import("Sofa.Core");
            py::object rootNode = sofapython3::PythonFactory::toPython(node->base()->toBaseNode());
            py::module m = py::module::import("SofaQtQuick");
            return py::cast<bool>(m.attr("createPrefabFromNode")(fileName, rootNode, name.toStdString(), help.toStdString()));
        }
    }
    return false;
}


}  // namespace qtquick
}  // namespace sofa
