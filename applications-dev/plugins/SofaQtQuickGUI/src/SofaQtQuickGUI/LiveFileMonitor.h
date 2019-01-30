/*
Copyright 2016,
Author: damien.marchal@univ-lille1.fr, Copyright 2016 CNRS.

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
#ifndef LIVEINTERFACEEDITOR_H
#define LIVEINTERFACEEDITOR_H

#include <QStringList>
#include <QObject>
#include <QFileSystemWatcher>

#include <SofaQtQuickGUI/config.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/FileMonitor.h>
using sofa::helper::system::FileEventListener;

class QQmlEngine;

namespace sofa
{

namespace qtquick
{

namespace livefilemonitor
{


/*******************************************************************************
 *  \class live file monitor.
 *  It monitor files at regular intervals and updates QML preview of them.
 ******************************************************************************/
class SOFA_SOFAQTQUICKGUI_API LiveFileMonitor : public QObject, public FileEventListener
{
public:
    Q_OBJECT

    Q_PROPERTY(QStringList files READ files NOTIFY filesChanged)
    //Q_INVOKABLE bool addFile(const QUrl&);

public:
    explicit LiveFileMonitor(QQmlEngine* q, QObject *parent = nullptr) ;
    virtual ~LiveFileMonitor() ;

    void fileHasChanged(const std::string& filename);
    QStringList files() const ;

private slots:
    void update() ;

signals:
    void filesChanged() ;

private:
    QQmlEngine*  m_engine;
    QStringList m_files;
    void addPathToMonitor(const std::string& filename);
};

}

using livefilemonitor::LiveFileMonitor ;

}

}

#endif // LIVEINTERFACEEDITOR_H
