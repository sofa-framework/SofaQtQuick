/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SofaApplication.h"

#include <QGuiApplication>
#include <QQuickWindow>
#include <QScreen>
#include <QDir>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

SofaApplication* SofaApplication::OurInstance = nullptr;

SofaApplication::SofaApplication(QObject* parent) : QObject(parent)
{
    OurInstance = this;
}

SofaApplication::~SofaApplication()
{
    if(this == OurInstance)
        OurInstance = nullptr;
}

SofaApplication* SofaApplication::Instance()
{
    return OurInstance;
}

QString SofaApplication::binaryDirectory() const
{
    return QCoreApplication::applicationDirPath();
}

void SofaApplication::saveScreenshot(const QString& path)
{
    if(!qGuiApp)
    {
        qWarning() << "Cannot take a screenshot of the whole application without an instance of QGuiApplication";
        return;
    }

    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if(!dir.exists())
        dir.mkpath(".");

    QString newPath = fileInfo.absoluteDir().absolutePath() + "/";
    newPath += fileInfo.baseName();
    QString suffix = "." + fileInfo.completeSuffix();

    QWindowList windows = qGuiApp->allWindows();
    QList<QQuickWindow*> quickWindows;
    for(QWindow* window : windows)
    {
        QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(window);
        if(quickWindow)
            quickWindows.append(quickWindow);
    }

    int count = -1;
    if(quickWindows.size() > 1)
        count = 0;

    bool saved = false;
    for(QQuickWindow* quickWindow : quickWindows)
    {
        QString finalPath = newPath;
        if(-1 != count)
            finalPath += "_" + QString::number(count++);

        finalPath += suffix;

        if(quickWindow->grabWindow().save(finalPath))
            saved = true;
    }

    if(!saved)
        qWarning() << "Screenshot could not be saved to" << path;
}

}

}
