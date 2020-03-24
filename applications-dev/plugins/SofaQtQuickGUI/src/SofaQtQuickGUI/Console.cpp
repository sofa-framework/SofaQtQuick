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
#include <sofa/helper/logging/Messaging.h>
#include <sofa/helper/logging/MessageDispatcher.h>
using sofa::helper::logging::MessageDispatcher ;

#include <sofa/helper/logging/MessageFormatter.h>
using sofa::helper::logging::MessageFormatter ;

#include <sofa/helper/logging/MessageHandler.h>
using sofa::helper::logging::MessageHandler ;

#include <sofa/helper/logging/Message.h>
using sofa::helper::logging::Message ;
#include <sofa/helper/logging/ComponentInfo.h>
using sofa::helper::logging::ComponentInfo;

#include <sofa/core/objectmodel/Base.h>
using sofa::helper::logging::SofaComponentInfo ;

using sofa::core::objectmodel::BaseObject ;
using sofa::core::objectmodel::BaseNode;

#include <SofaQtQuickGUI/Console.h>

#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
using sofa::qtquick::SofaComponent ;

#include <SofaQtQuickGUI/SofaBaseApplication.h>
using sofaqtquick::SofaBaseApplication ;

namespace sofaqtquick
{

namespace console
{

Console::Console(QObject *parent) : QAbstractListModel(parent)
{
    MessageDispatcher::addHandler(this);
}

Console::~Console()
{
    MessageDispatcher::rmHandler(this) ;
}

void Console::process(Message &m)
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.push_back(m) ;
    switch (m.type())
    {
    case Message::Type::Fatal:
        m_fatals.push_back(&m);
        break;
    case Message::Type::Error:
        m_errors.push_back(&m);
        break;
    case Message::Type::Warning:
        m_warnings.push_back(&m);
        break;
    case Message::Type::Deprecated:
        m_deprecated.push_back(&m);
        break;
    default: break;
    }
    endInsertRows();

    //if(m_messages.size() > 50){
    //    m_messages.erase(m_messages.begin(),m_messages.begin()+1);
    //}

    emit messageCountChanged();
}

void Console::clear()
{
    beginResetModel();
    m_messages.clear() ;
    m_fatals.clear() ;
    m_errors.clear() ;
    m_warnings.clear() ;
    m_deprecated.clear() ;
    endResetModel();
    emit messageCountChanged();
}


int Console::rowCount(const QModelIndex & /*parent*/) const
{
    return getMessageCount();
}

int Console::getMessageCount() const
{
    return int(m_messages.size());
}

QVariant Console::data(const QModelIndex& index, int role) const
{

    if(!index.isValid())
    {
        msg_error("SofaQtQuickGUI") << "Invalid index";
        return QVariant("");
    }

    if((unsigned int)index.row() >= m_messages.size())
    {
        msg_error("SofaQtQuickGUI") << "Index out of bound";
        return QVariant("");
    }

    const Message& item = m_messages[index.row()];

    switch(role)
    {
    case MSG_MESSAGE:
        return QVariant::fromValue(QString::fromStdString(item.message().str()));
    case MSG_FILE:
        return QVariant::fromValue(QString::fromStdString(item.fileInfo()->filename));
    case MSG_LINE:
        return QVariant::fromValue(item.fileInfo()->line);
    case MSG_TYPE:
        return QVariant::fromValue((int)item.type());
    case MSG_EMITTER_PATH:{
        SofaComponentInfo* nfo = dynamic_cast<SofaComponentInfo*>(item.componentInfo().get()) ;
        if( nfo != nullptr )
        {
            if( nfo->m_component!=nullptr ){
                const BaseObject* object = nfo->m_component->toBaseObject() ;
                if(object)
                    return QVariant::fromValue(QString::fromStdString(object->getPathName())) ;
                const BaseNode* context = nfo->m_component->toBaseNode() ;
                if(context)
                    return QVariant::fromValue(QString::fromStdString(context->getPathName())) ;
            }
        }
        return QVariant::fromValue(nullptr);
    }
    case MSG_EMITTER:{
        SofaComponentInfo* nfo = dynamic_cast<SofaComponentInfo*>(item.componentInfo().get()) ;
        if( nfo != nullptr )
        {
            const std::string& classname= nfo->sender();
            const std::string& name = nfo->name();
            return QVariant::fromValue(QString::fromStdString(classname + "(" + name + ")"));
        }
        return QVariant::fromValue(QString::fromStdString(item.sender()));
    }
    default:
        msg_error("SofaQtQuickGUI") << "Role unknown (this shouldn't happen in an official release and "
                                       "must be fixed by careful code-path analysis and test " ;
    }

    return QVariant("");
}


QHash<int,QByteArray> Console::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[MSG_MESSAGE]     = "message";
    roles[MSG_EMITTER]     = "emitter";
    roles[MSG_EMITTER_PATH]= "emitterpath";
    roles[MSG_TYPE]        = "type";
    roles[MSG_LINE]        = "line";
    roles[MSG_FILE]        = "link";
    return roles;
}

}  // namespace console

}  // namspace sofaqtquick
