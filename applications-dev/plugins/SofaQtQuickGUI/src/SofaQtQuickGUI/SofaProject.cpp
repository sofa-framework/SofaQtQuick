#include "SofaProject.h"
#include "SofaApplication.h"

#ifdef SOFAQTQUICK_WITH_SOFAPYTHON3
#include <SofaPython3/PythonEnvironment.h>
using sofapython3::PythonEnvironment;
namespace py = pybind11;
#else
#include <SofaPython/PythonEnvironment.h>
#include <SofaPython/PythonFactory.h>
using sofa::simulation::PythonEnvironment;
#endif
#include <QWindow>
#include <QInputDialog>
#include <QFileDialog>

#include <fstream>

namespace sofa {
namespace qtquick {

SofaProject::SofaProject() {}
SofaProject::~SofaProject() { if (m_fileMonitor) delete m_fileMonitor; }

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
    QDir root = QDir(m_rootDir.path());
    scanProject(root);
}

void SofaProject::onFilesChanged()
{
    for (const auto& f : m_fileMonitor->files())
    {
        std::cout << f.toStdString() << " changed" << std::endl;
        QFileInfo finfo(f);
        m_assets.erase(m_assets.find(finfo.filePath()));
        m_assets.insert(assetMapPair(finfo.filePath(), AssetFactory::createInstance(
                                         finfo.filePath(), finfo.suffix())));
    }
    emit filesChanged();
}

void SofaProject::scanProject(const QUrl& folder)
{
    _scanProject(QDir(folder.path()));
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

void SofaProject::scanProject(const QDir& folder)
{
    _scanProject(folder);
}

void SofaProject::_scanProject(const QDir& folder)
{
    if (!folder.exists())
        return;


    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    QStringList content = folder.entryList();
    QFileInfoList contentInfo = folder.entryInfoList();
    for (int idx = 0 ; idx < content.size() ; ++idx)
    {
        QFileInfo f = contentInfo[idx];
        if (f.isDir())
        {
            if (f.fileName() == "." || f.fileName() == "..")
                continue;
            _scanProject(QDir(f.filePath()));
        }
        else
        {
            m_assets.insert(assetMapPair(f.filePath(), AssetFactory::createInstance(
                                             f.filePath(), f.suffix())));
        }
    }
    m_fileMonitor = new LiveFileMonitor(m_rootDir.path());
    connect(m_fileMonitor, &LiveFileMonitor::filesChanged, this, &SofaProject::onFilesChanged);
    QApplication::restoreOverrideCursor();
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
#ifndef SOFAQTQUICK_WITH_SOFAPYTHON3
            PythonEnvironment::gil lock(__func__);
            PyObject* file = PyString_FromString(fileName.c_str());
            PyObject* rootNode = sofa::PythonFactory::toPython(node->base()->toBaseNode());
            PyObject* n = PyString_FromString(name.toStdString().c_str());
            PyObject* h = PyString_FromString(help.toStdString().c_str());
            PyObject* args = PyTuple_Pack(4, file, rootNode, n, h);
            PyObject* ret = PythonEnvironment::callObject("createPrefabFromNode", "SofaPython", args);
            return PyObject_IsTrue(ret);
#else
            py::str file(fileName);
            py::object rootNode = py::cast(node->base()->toBaseNode());
            py::str n(name.toStdString());
            py::str h(help.toStdString());
            py::tuple args = py::make_tuple(file, rootNode, n ,h);
            py::module m = py::module::import("SofaQtQuick");
            return py::cast<bool>(m.attr("createPrefabFromNode")(args));
#endif
        }
    }
    return false;
}


}  // namespace qtquick
}  // namespace sofa
