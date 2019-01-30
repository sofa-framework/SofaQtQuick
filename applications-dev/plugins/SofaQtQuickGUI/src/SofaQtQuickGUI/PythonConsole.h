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

#ifndef PYTHONCONSOLE_H
#define PYTHONCONSOLE_H

#include <SofaPython/PythonMacros.h>

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>
#include <QObject>
#include <QString>
#include <QSet>

namespace sofa
{

namespace qtquick
{

/// \class A basic Python console to be able to communicate with our sofa python scene using text commands
class SOFA_SOFAQTQUICKGUI_API PythonConsole : public QObject
{
    Q_OBJECT

public:
    PythonConsole(QObject *parent = 0);
    ~PythonConsole();

signals:
    void textAdded(const QString& text);

public:
    static void AddText(const QString& text);

private:
    static void InitConsoleRedirection();

private:
    static QSet<PythonConsole*> OurPythonConsoles;

};

}

}

#endif // PYTHONCONSOLE_H
