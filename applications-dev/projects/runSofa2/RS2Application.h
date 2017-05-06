/*
Copyright 2015, Anatoscope
Copyright 2017, CNRS

This file is part of runSofa2.

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.

Contributors:
    - anatoscope.
    - damien.marchal@univ-lille1.fr.
*/

#ifndef RS2APPLICATION_H
#define RS2APPLICATION_H

#include "SofaQtQuickGUI/DefaultApplication.h"

namespace sofa
{
namespace rs
{
namespace _rs2application_
{

using sofa::qtquick::DefaultApplication ;

class RS2Application : public DefaultApplication
{
public:
    Q_OBJECT

public:
    RS2Application(QObject* parent = 0);
    virtual ~RS2Application();

    /// Centralized common settings for most sofaqtquick applications.
    /// Every applications will be updated when modifying this code.
    /// To be called in the main function.
    bool mainInitialization(QApplication& app,
                            QQmlApplicationEngine& applicationEngine ,
                            const QString &mainScript) override ;

};

} /// namespace _rs2application_

using sofa::rs::_rs2application_::RS2Application ;

} /// namespace rs
} /// namespace sofa

#endif // RS2APPLICATION_H
