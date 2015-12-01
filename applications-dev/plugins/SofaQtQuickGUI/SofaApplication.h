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

#ifndef SOFAAPPLICATION_H
#define SOFAAPPLICATION_H

#include "SofaQtQuickGUI.h"

namespace sofa
{

namespace qtquick
{

class SOFA_SOFAQTQUICKGUI_API SofaApplication : public QObject
{
    Q_OBJECT

public:

public:
    static SofaApplication* Instance();

    Q_INVOKABLE QString binaryDirectory() const;
    Q_INVOKABLE void saveScreenshot(const QString& path);

protected:
    SofaApplication(QObject* parent = 0);

public:
    ~SofaApplication();

private:
    static SofaApplication* OurInstance;

};

}

}

#endif // SOFAAPPLICATION_H
