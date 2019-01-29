/*
Copyright 2016 CNRS.

contributors:
    - damien.marchal@univ-lille1.fr

This file is part of Sofa

Sofa is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Sofa. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QFile>
#include <QTimer>

#include <QQmlEngine>

#include <sofa/helper/logging/Messaging.h>

#include "SofaApplication.h"
using sofa::qtquick::SofaApplication ;

#include <sofa/helper/system/FileMonitor.h>
using sofa::helper::system::FileMonitor ;
using sofa::helper::system::FileEventListener ;

#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/FileMonitor.h>
using sofa::helper::system::FileSystem;
using sofa::helper::system::FileMonitor;

#include "LiveFileMonitor.h"

namespace sofa
{

namespace qtquick
{

namespace livefilemonitor
{

void LiveFileMonitor::addPathToMonitor(const std::string& path)
{
    std::string p = path;
    if (p.back() == '/')
        p.pop_back();
    std::vector<std::string> files;
    FileSystem::listDirectory(p, files) ;
    for(auto& filename: files)
    {
        if (FileSystem::isDirectory(p + "/" + filename))
        {
            addPathToMonitor(p + "/" + filename) ;
        }
        else
        {
            if (FileSystem::isFile(p + "/" + filename) && FileSystem::getExtension(filename) == "qml")
            {
                sofa::helper::system::FileMonitor::addFile(p + "/" + filename, this);
            }
        }
    }
}

LiveFileMonitor::LiveFileMonitor(QQmlEngine *engine, QObject *parent)
{
    Q_UNUSED(parent);

    msg_info("LiveFileMonitor") << "Creating a LiveFileMonitor singleton with data directory '"
                                << SOFAQTQUICKGUI_QML_DIR << "'" ;
    m_files = QStringList() ;

    std::string path(SOFAQTQUICKGUI_QML_DIR);
    addPathToMonitor(path);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(200);

    m_engine = engine ;
}

LiveFileMonitor::~LiveFileMonitor()
{
}

void LiveFileMonitor::update()
{
    FileMonitor::updates(0) ;
}

void LiveFileMonitor::fileHasChanged(const std::string& filename)
{
    m_engine->clearComponentCache();
    m_files.clear();
    m_files.push_back(QString::fromStdString(filename));
    emit filesChanged();
}

/*
bool LiveFileMonitor::addFile(const QUrl &filename)
{
    return FileMonitor::addFile(filename.toLocalFile(), m_filelistener) >= 0;
}*/

QStringList LiveFileMonitor::files() const
{
    return m_files ;
}


}

}

}
