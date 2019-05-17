/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

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
*********************************************************************/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/
#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantMap>

#include <SofaQtQuickGUI/config.h>
#include <sofa/core/objectmodel/BaseData.h>

namespace sofaqtquick::bindings
{
    namespace _sofadata_
    {
        using sofa::core::objectmodel::BaseData;

        class SofaData : public QObject
        {
            Q_OBJECT

            Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
            Q_PROPERTY(QString help READ getHelp NOTIFY helpChanged)
            Q_PROPERTY(QString valueType READ getValueType NOTIFY valueTypeChanged)
            Q_PROPERTY(QString group READ getGroup)
            Q_PROPERTY(QVariantMap properties READ getProperties NOTIFY propertiesChanged)
            Q_PROPERTY(bool isReadOnly READ isReadOnly NOTIFY readOnlyChanged)

            Q_PROPERTY(QString linkPath READ getLinkPath NOTIFY linkPathChanged)
            Q_PROPERTY(QVariant value READ getValue WRITE setValue NOTIFY valueChanged)

        public:
            SofaData(BaseData* self);

            Q_INVOKABLE QString getName() const;
            Q_INVOKABLE QString getPathName() const;
            Q_INVOKABLE QVariant getValue() const;
            Q_INVOKABLE QString getValueType() const;

            Q_INVOKABLE QString getHelp() const;
            Q_INVOKABLE bool isSet() const;
            Q_INVOKABLE bool isReadOnly() const;
            Q_INVOKABLE QString getGroup() const;

            Q_INVOKABLE bool setValue(const QVariant& getValue);
            Q_INVOKABLE bool setLink(const QString& path);
            Q_INVOKABLE QString getLinkPath()const;

            [[deprecated("Remove, use directly the object")]]
            Q_INVOKABLE QVariantMap object();

            Q_INVOKABLE QVariantMap getProperties() const;

            BaseData* rawData() { return m_self; }
            const BaseData* rawData() const { return m_self; }

        signals:
            void valueChanged(const QVariant& newValue);
            void readOnlyChanged(const bool);
            void valueTypeChanged(const QString newValue);
            void helpChanged(const QString newValue);
            void nameChanged(const QString newValue);
            void linkPathChanged(const QString newValue);
            void propertiesChanged(const QVariantList newValues);
        private:
            BaseData* m_self {nullptr};
        };
    }

    /// Import SofaData from its private namespace into the public one.
    using _sofadata_::SofaData;
}

