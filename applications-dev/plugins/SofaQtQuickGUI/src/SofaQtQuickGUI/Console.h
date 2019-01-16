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
#include <QAbstractListModel>

#include <sofa/helper/vector.h>

#include <sofa/helper/logging/MessageHandler.h>
/// Forward declaration
namespace sofa {
    namespace helper {
        namespace logging {
            class Message;
        }
    }
    namespace qtquick {
        class SofaScene;
        class SofaComponent;
    }
}

namespace sofa {
namespace qtquick {
namespace console {

// I can use 'using' because I'm in my private 'console' namespace so it
// will not generate namespace pollution.
using sofa::helper::logging::Message ;
using sofa::helper::logging::MessageHandler ;

/*******************************************************************************
 *  \class A console with features to interact on the messages.
 *    1) click on the message location to expand it
 *    2) click on the source code location to open it in a source editor
 *    3) popup menu to select the emitting object.
 ******************************************************************************/
class SOFA_SOFAQTQUICKGUI_API Console : public QAbstractListModel,
                                        public MessageHandler
{
    Q_OBJECT

    enum {
        MSG_MESSAGE = 0,
        MSG_EMITTER,
        MSG_EMITTER_PATH,
        MSG_FILE,
        MSG_LINE,
        MSG_TYPE,
        MSG_ROLES_COUNT
    }ConsoleRoles;

public:
    Console(QObject *parent = 0);
    ~Console();

    Q_PROPERTY(int messageCount READ getMessageCount NOTIFY messageCountChanged)
    Q_PROPERTY(SofaScene* sofaScene READ sofaScene WRITE setSofaScene NOTIFY sofaSceneChanged)
    Q_PROPERTY(SofaComponent* filter READ filter WRITE setFilter NOTIFY filterChanged)

    /// Set the scene associated with a message queue.
    SofaScene* m_sofaScene {nullptr} ;
    SofaScene* sofaScene(){ return m_sofaScene; }
    void setSofaScene(SofaScene* s){ m_sofaScene = s; }

    SofaComponent* m_filter {nullptr} ;
    /// Set a component to filter the message to return in the engine.
    SofaComponent* filter(){ return m_filter; }
    void setFilter(SofaComponent* c){ m_filter = c; }


    /// inherited from QAbstractListModel
    int rowCount(const QModelIndex& parent) const ;
    QVariant data(const QModelIndex& index, int role) const ;
    QHash<int, QByteArray> roleNames() const;

    /// inherited from MessageHandler
    virtual void process(Message &m) ;

    int getMessageCount() const ;

    Q_INVOKABLE void clear() ;

signals:
    void messageCountChanged() ;
    void sofaSceneChanged() ;
    void filterChanged() ;

private:
    sofa::helper::vector<Message> m_messages ;
};

}

}

}

#endif // CONSOLE_H
