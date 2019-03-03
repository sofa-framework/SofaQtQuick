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
            Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

        public:
            SofaData(BaseData* self);

            QVariant value() const;
            bool setValue(const QVariant& value);

        signals:
            void valueChanged(const QVariant& newValue);

        private:
            BaseData* m_self {nullptr};
        };
    }

    /// Import SofaData from its private namespace into the public one.
    using _sofadata_::SofaData;
}

#include <sofa/simulation/Simulation.h>
#include <QVariantMap>

class QTimer;
class QOpenGLShaderProgram;

namespace sofa
{

namespace qtquick
{

class SofaScene;
class SofaComponent;

/// QtQuick wrapper for a sofa data (i.e baseData), allowing us to share a component data in a QML context
class SOFA_SOFAQTQUICKGUI_API SofaData : public QObject
{
    Q_OBJECT

public:
    SofaData(const SofaComponent* sofaComponent, const sofa::core::objectmodel::BaseData* data);
    SofaData(SofaScene* sofaScene, const sofa::core::objectmodel::Base* base, const sofa::core::objectmodel::BaseData* data);
    SofaData(const SofaData& sceneData);

    Q_INVOKABLE sofa::qtquick::SofaComponent* sofaComponent() const;

    Q_INVOKABLE QVariantMap object();

    Q_INVOKABLE QVariant value() const;
    Q_INVOKABLE bool setValue(const QVariant& value);
    Q_INVOKABLE bool setLink(const QString& path);

    sofa::core::objectmodel::BaseData* data();
    const sofa::core::objectmodel::BaseData* data() const;

private:
    SofaComponent*                              mySofaComponent;
    const sofa::core::objectmodel::BaseData*    myData;

};

}

}

