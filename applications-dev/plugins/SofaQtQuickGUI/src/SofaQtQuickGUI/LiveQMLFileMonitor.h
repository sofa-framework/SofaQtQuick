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

#include "LiveFileMonitor.h"
#include <QStringList>
#include <QObject>

class QQmlEngine;

namespace sofaqtquick
{

namespace liveqmlfilemonitor
{


/*******************************************************************************
 *  \class live file monitor.
 *  It monitor files at regular intervals and updates QML preview of them.
 ******************************************************************************/
class SOFA_SOFAQTQUICKGUI_API LiveQMLFileMonitor : public LiveFileMonitor
{
public:
    Q_OBJECT

public:
    explicit LiveQMLFileMonitor(QQmlEngine* q, QObject *parent = nullptr);
    virtual ~LiveQMLFileMonitor() ;

private:
    QQmlEngine*  m_engine;
};

}  // namespace liveqmlfilemonitor

using liveqmlfilemonitor::LiveQMLFileMonitor ;

} // namespace sofaqtquick


#endif // LIVEINTERFACEEDITOR_H
