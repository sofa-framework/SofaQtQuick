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
#ifndef LIVEFILEMONITOR_H
#define LIVEFILEMONITOR_H

#include <QStringList>
#include <QObject>
#include <QFileSystemWatcher>

#include <SofaQtQuickGUI/config.h>
#include <sofa/helper/system/FileSystem.h>
#include <sofa/helper/system/FileMonitor.h>
using sofa::helper::system::FileEventListener;

class QQmlEngine;

namespace sofaqtquick
{
namespace _livefilemonitor
{

/*******************************************************************************
 *  \class live file monitor.
 *  It monitor files at regular intervals and sends the list of changed files through a signal.
 ******************************************************************************/
class SOFA_SOFAQTQUICKGUI_API LiveFileMonitor : public QObject, public FileEventListener
{
public:
    Q_OBJECT

    Q_PROPERTY(QStringList files READ files NOTIFY filesChanged)
    Q_PROPERTY(QString root READ root WRITE setRoot NOTIFY rootChanged)

public:
    explicit LiveFileMonitor(const QString& root, int msec = 200, QObject *parent = nullptr);
    virtual ~LiveFileMonitor();

    void fileHasChanged(const std::string& filename);
    QStringList files() const;
    QString root() const;
    void setRoot(const QString&);

private slots:
    void update();

signals:
    void filesChanged();
    void rootChanged();

protected:
    QStringList m_files;
    QString m_root;
    void addPathToMonitor(const std::string& filename);
};

}  // namespace _livefilemonitor

using _livefilemonitor::LiveFileMonitor;

}  // namespace sofaqtquick

#endif // LIVEFILEMONITOR_H
