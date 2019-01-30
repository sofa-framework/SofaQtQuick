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

#ifndef SOFAPROCESSSTATE_H
#define SOFAPROCESSSTATE_H

#include <SofaQtQuickGUI/config.h>

#include <QProcess>

namespace sofa
{

namespace qtquick
{

/// \class State of a process
class SOFA_SOFAQTQUICKGUI_API ProcessState : public QObject
{
    Q_OBJECT

public:
    ProcessState(QProcess* process) : QObject(),
        myProcess(process)
    {
        connect(myProcess, SIGNAL(finished(int)), this, SIGNAL(finished(int)));
    }

public:
    Q_INVOKABLE bool isRunning() { return QProcess::NotRunning != myProcess->state(); }
    Q_INVOKABLE int exitCode() const { return myProcess->exitCode(); }
    Q_SLOT bool waitForFinished(int msecs) { return myProcess->waitForFinished(msecs); }

    Q_SLOT void terminate() { myProcess->terminate(); }
    Q_SLOT void kill() { myProcess->kill(); }

    Q_SLOT void destroyProcess() { delete myProcess; myProcess = nullptr; }

signals:
    void finished(int exitCode);

private:
    QProcess*   myProcess;

};

}

}

#endif // SOFAPROCESSSTATE_H
