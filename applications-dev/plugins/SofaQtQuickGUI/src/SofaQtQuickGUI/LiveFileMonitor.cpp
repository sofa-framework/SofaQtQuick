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

#include <SofaQtQuickGUI/SofaBaseApplication.h>
using sofaqtquick::SofaBaseApplication ;

#include <sofa/helper/system/FileMonitor.h>
using sofa::helper::system::FileMonitor ;
using sofa::helper::system::FileEventListener ;

#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/FileMonitor.h>
using sofa::helper::system::FileSystem;
using sofa::helper::system::FileMonitor;

#include "LiveFileMonitor.h"

namespace sofaqtquick::_livefilemonitor
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
            if (FileSystem::isFile(p + "/" + filename))
            {
                sofa::helper::system::FileMonitor::addFile(p + "/" + filename, this);
            }
        }
    }
}

LiveFileMonitor::LiveFileMonitor(const QString& root, int msec, QObject *parent)
{
    msg_info("LiveFileMonitor") << "Creating a LiveFileMonitor singleton with data directory '"
                                << root.toStdString() << "'" ;
    Q_UNUSED(parent);

    m_files = QStringList() ;

    std::string path(root.toStdString());
    addPathToMonitor(path);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(msec);
}

LiveFileMonitor::~LiveFileMonitor()
{
    sofa::helper::system::FileMonitor::removeListener(this);
}

void LiveFileMonitor::update()
{
    FileMonitor::updates(0) ;
}

void LiveFileMonitor::fileHasChanged(const std::string& filename)
{
    m_files.clear();
    m_files.push_back(QString::fromStdString(filename));
    emit filesChanged();
}

QStringList LiveFileMonitor::files() const
{
    return m_files ;
}

QString LiveFileMonitor::root() const
{
    return m_root;
}

void LiveFileMonitor::setRoot(const QString& root)
{
    msg_info("LiveMonitor") << "setRoot to: " << root.toStdString() ;
    m_root = root;
    sofa::helper::system::FileMonitor::removeListener(this);
    addPathToMonitor(m_root.toStdString());
}

}  // namespace sofaqtquick::_livefilemonitor
