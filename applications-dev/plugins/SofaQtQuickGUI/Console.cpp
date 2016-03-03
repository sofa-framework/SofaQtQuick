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
#include "sofa/helper/logging/Messaging.h"
#include "sofa/helper/logging/MessageDispatcher.h"
using sofa::helper::logging::MessageDispatcher ;

#include "sofa/helper/logging/MessageFormatter.h"
using sofa::helper::logging::MessageFormatter ;

#include "sofa/helper/logging/MessageHandler.h"
using sofa::helper::logging::MessageHandler ;

#include "sofa/helper/logging/Message.h"
using sofa::helper::logging::Message ;

#include "Console.h"


namespace sofa
{

namespace qtquick
{

namespace console
{

QtAppMessageHandler::QtAppMessageHandler(Console* console)
{
    m_console = console ;
}

void QtAppMessageHandler::process(Message &m){
    m_console->messageAdded( QString(m.sender().c_str()),
                             QString(m.message().str().c_str()),
                             QString(m.fileInfo().filename),
                             m.type(),
                             m.fileInfo().line) ;
}

Console::Console(QObject *parent) : QObject(parent)
{
    MessageDispatcher::addHandler(new QtAppMessageHandler(this)) ;
}

Console::~Console()
{
    //MessageDispatcher::rmHandler()
}


}

}

}
