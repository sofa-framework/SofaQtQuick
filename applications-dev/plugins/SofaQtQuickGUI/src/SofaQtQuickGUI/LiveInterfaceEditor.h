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

#include <SofaQtQuickGUI/config.h>

namespace sofa
{

namespace qtquick
{

namespace livefilemonitor
{

/*******************************************************************************
 *  \class live interface editor.
 *  It monitor files at regular intervals and updates QML preview of them.
 ******************************************************************************/
class SOFA_SOFAQTQUICKGUI_API LiveFileMonitor : public QObject
{
public:
    Q_OBJECT


public:
    explicit LiveFileMonitor(QObject *parent = nullptr) ;
    virtual ~LiveFileMonitor() ;

private:
};

}

using livefilemonitor::LiveFileMonitor ;

}

}

#endif // LIVEINTERFACEEDITOR_H
