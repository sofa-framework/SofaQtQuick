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

#include "LiveQMLFileMonitor.h"

namespace sofaqtquick
{

namespace liveqmlfilemonitor
{


LiveQMLFileMonitor::LiveQMLFileMonitor(QQmlEngine *engine, QObject *parent)
    : LiveFileMonitor (SOFAQTQUICKGUI_QML_DIR, 200, parent)
{
    m_files = QStringList() ;

    std::string path(SOFAQTQUICKGUI_QML_DIR);
    addPathToMonitor(path);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(200);

    m_engine = engine ;
}

LiveQMLFileMonitor::~LiveQMLFileMonitor()
{
}

}  // namespace liveqmlfilemonitor

}  // namespace sofaqtquick
