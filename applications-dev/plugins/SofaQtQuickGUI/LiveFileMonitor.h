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
#define LIVEINTERFACEEDITOR_H

#include <QStringList>
#include <QObject>

#include "SofaQtQuickGUI.h"

class QQmlEngine;

/// FORWARD DECLARATIONb
namespace sofa{
    namespace helper{
        namespace system{
            class FileEventListener ;
        }
    }
}

namespace sofa
{

namespace qtquick
{

namespace livefilemonitor
{

using sofa::helper::system::FileEventListener ;

/*******************************************************************************
 *  \class live file monitor.
 *  It monitor files at regular intervals and updates QML preview of them.
 ******************************************************************************/
class SOFA_SOFAQTQUICKGUI_API LiveFileMonitor : public QObject
{
public:
    Q_OBJECT

    Q_PROPERTY(QStringList files READ files NOTIFY filesChanged)

public:
    explicit LiveFileMonitor(QQmlEngine* q, QObject *parent = nullptr) ;
    virtual ~LiveFileMonitor() ;

    QStringList files() const ;

    void hasChanged(const std::string&);

private slots:
    void update() ;

signals:
    void filesChanged() ;

private:
    QQmlEngine*  m_engine;
    QStringList m_files;
    FileEventListener* m_filelistener {nullptr};
};

}

using livefilemonitor::LiveFileMonitor ;

}

}

#endif // LIVEINTERFACEEDITOR_H
