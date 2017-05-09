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

#include <QCommandLineParser>
#include <QWindow>
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
    Q_PROPERTY(bool animateOnLoad READ getAnimateOnLoad CONSTANT) ;
    Q_PROPERTY(QString defaultScene READ getDefaultScene CONSTANT) ;
    Q_PROPERTY(int defaultWidth READ getDefaultWidth CONSTANT) ;
    Q_PROPERTY(int defaultHeight READ getDefaultHeight CONSTANT) ;
    Q_PROPERTY(QWindow::Visibility defaultVisibility READ getDefaultVisibility CONSTANT) ;

    RS2Application(QObject* parent = 0);
    virtual ~RS2Application();

    /// Initialize the application, parse command line argument and expose everything as
    /// a qml component name RS2Application.
    bool mainInitialization(QApplication& app,
                            QQmlApplicationEngine& applicationEngine ,
                            const QString &mainScript) override ;

    bool getAnimateOnLoad() ;
    int getDefaultWidth() ;
    int getDefaultHeight() ;
    QString getDefaultScene() ;
    QWindow::Visibility getDefaultVisibility() ;

protected:
    void processCommandLineOptions(const QStringList &app) ;
    void setMessageType(const QString& type) ;
    void setConsoleMode(const QString& mode) ;
    void loadSofaPlugins(const QStringList& plugins);

private:
    bool m_animateOnLoad      {false} ;
    int m_defaultWidth        {1280} ;
    int m_defaultHeight       {720} ;
    QWindow::Visibility m_defaultVisibility {QWindow::Windowed} ;
    QString m_defaultScene    {"file:Demos/caduceus.scn"} ;
};

} /// namespace _rs2application_

using sofa::rs::_rs2application_::RS2Application ;

} /// namespace rs
} /// namespace sofa

#endif // RS2APPLICATION_H
