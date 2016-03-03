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
#ifndef CONSOLE_H
#define CONSOLE_H

#include "SofaQtQuickGUI.h"
#include <QObject>
#include <QString>
#include <QSet>

#include <sofa/helper/logging/MessageHandler.h>

/// Forward declaration
namespace sofa {
    namespace helper {
        namespace logging {
            class Message;
        }
    }
}

namespace sofa
{

namespace qtquick
{

namespace console
{
class Console ;

// I can use 'using' because I'm in my private 'console' namespace so it
// will not generate namespace pollution.
using sofa::helper::logging::Message ;
using sofa::helper::logging::MessageHandler ;


class QtAppMessageHandler : public MessageHandler
{
    Console* m_console ;
public:
    QtAppMessageHandler(Console* console) ;
    virtual void process(Message &m) ;
};

/*******************************************************************************
 *  \class A console with features to interact on the messages.
 *    1) click on the message location to expand it
 *    2) click on the source code location to open it in a source editor
 *    3) popup menu to select the emitting object.
 ******************************************************************************/
class SOFA_SOFAQTQUICKGUI_API Console : public QObject
{
    Q_OBJECT

public:
    Console(QObject *parent = 0);
    ~Console();

signals:
    void messageAdded(const QString& emitter,
                      const QString& message,
                      const QString& source,
                      int type, int line);
};

namespace singleton
{
    class Console
    {
        public:
            static sofa::qtquick::console::Console* instance() ;
    };
}

}

}

}

#endif // CONSOLE_H
