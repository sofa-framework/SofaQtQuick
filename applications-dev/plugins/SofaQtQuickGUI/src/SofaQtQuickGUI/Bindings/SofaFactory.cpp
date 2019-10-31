/*
Copyright 2018,
Author: damien.marchal@univ-lille1.fr, Copyright 2018 CNRS.

This file is part of SofaQtQuick

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

#include <sofa/core/ObjectFactory.h>
using sofa::core::ObjectFactory ;

#include <SofaQtQuickGUI/Bindings/SofaFactory.h>
#include <SofaQtQuickGUI/Bindings/SofaBaseObject.h>

#include <SofaSimulationGraph/SimpleApi.h>
#include <tuple>

namespace sofaqtquick::bindings
{

namespace _sofafactory_
{

SofaFactory::SofaFactory(QObject *parent) : QObject(parent)
{
    setFilter("");
}

SofaFactory::~SofaFactory(){}

bool SofaFactory::contains(const QString& name)
{
    /// Recompute the filtered list.
    std::vector<ObjectFactory::ClassEntry::SPtr> entries ;
    ObjectFactory::getInstance()->getAllEntries(entries) ;

    for (size_t i=0; i<entries.size(); i++)
    {
        QString cname = QString::fromStdString(entries[i]->className) ;
        if( cname == name )
            return true;
    }
    return false;
}

void SofaFactory::setFilter(const QString& s)
{
    m_filter = s ;

    /// Recompute the filtered list.
    std::vector<ObjectFactory::ClassEntry::SPtr> entries ;
    ObjectFactory::getInstance()->getAllEntries(entries) ;

    m_filteredList.clear();
    for (size_t i=0; i<entries.size(); i++){
        QString cname = QString::fromStdString(entries[i]->className) ;
        if( m_filter.isEmpty() || cname.contains(m_filter) )
            m_filteredList.push_back(cname);
    }

    emit listChanged() ;
}

QStringList SofaFactory::getComponents()
{
    return m_filteredList ;
}

QString SofaFactory::getComponentHelp(const QString& name)
{
    /// Recompute the filtered list.
    std::vector<ObjectFactory::ClassEntry::SPtr> entries ;
    ObjectFactory::getInstance()->getAllEntries(entries) ;

    QString s;
    for (size_t i=0; i<entries.size(); i++)
    {
        QString cname = QString::fromStdString(entries[i]->className) ;
        if( cname == name )
        {
            s = QString::fromStdString(entries[i]->description);
            s += "Default template: " + QString::fromStdString(entries[i]->defaultTemplate);
            break;
        }
    }
    return s;
}

QStringList SofaFactory::getComponentTemplates(const QString& name)
{
    /// Recompute the filtered list.
    std::vector<ObjectFactory::ClassEntry::SPtr> entries ;
    ObjectFactory::getInstance()->getAllEntries(entries) ;

    QStringList templates;
    for (size_t i=0; i<entries.size(); i++)
    {
        QString cname = QString::fromStdString(entries[i]->className) ;
        if( cname == name )
        {
            for (auto const& [key, value] : entries[i]->creatorMap)
            {
                SOFA_UNUSED(value);
                templates.push_back(QString::fromStdString(key));
            }
            break;
        }
    }
    return templates;
}

SofaNode* SofaFactory::createNode(const QString name) const
{
    return new SofaNode(name);
}


} /// _sofafactory_
} /// sofaqtquick::bindings

