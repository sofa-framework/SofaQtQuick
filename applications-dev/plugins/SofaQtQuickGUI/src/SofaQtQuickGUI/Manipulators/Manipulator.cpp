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

#include <SofaQtQuickGUI/Manipulators/Manipulator.h>
#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaOpenglVisual/OglModel.h>

#include <QMatrix4x4>
#include <QDebug>

namespace sofaqtquick
{

using bindings::SofaData;
using ::sofa::defaulttype::AbstractTypeInfo;
using ::sofa::core::objectmodel::BaseObject;

Manipulator::Manipulator(QObject* parent)
    : QObject(parent),
      myVisible(true),
      myPosition(nullptr),
      myOrientation(nullptr),
      myScale(nullptr)
{
    connect(this, &Manipulator::positionChanged, this, &Manipulator::onValueChanged);
    connect(this, &Manipulator::orientationChanged, this, &Manipulator::onValueChanged);
    connect(this, &Manipulator::scaleChanged, this, &Manipulator::onValueChanged);
}

Manipulator::~Manipulator()
{

}

void Manipulator::setVisible(bool newVisible)
{
    if(newVisible == myVisible)
        return;

    myVisible = newVisible;

    visibleChanged(newVisible);
}

void Manipulator::setName(const QString& newName)
{
    if(newName == myName)
        return;

    myName = newName;

    nameChanged(newName);
}


sofaqtquick::bindings::SofaBase* Manipulator::getSofaObject()
{
    return mySofaObject;
}

void Manipulator::setSofaObject(sofaqtquick::bindings::SofaBase* sofaObject)
{
    if (mySofaObject == sofaObject && mySofaObject->rawBase() == sofaObject->rawBase())
        return;

    mySofaObject = sofaObject;

    if (mySofaObject->rawBase()->getClassName() == "DAGNode")
    {
        std::cout << "got a DAGNode" << std::endl;
    }
    if (mySofaObject->rawBase()->getClassName() == "TransformEngine")
    {
        std::cout << "got a TransformEngine" << std::endl;
    }
    if (mySofaObject->rawBase()->getClassName() == "MechanicalObject")
    {
        std::cout << "got a MechanicalObject" << std::endl;
        setPositionData(qobject_cast<sofaqtquick::bindings::SofaData*>(sofaObject->getData("translation")));
    }
    if (mySofaObject->rawBase()->getClassName() == "OglModel")
    {
        std::cout << "got an OglModel" << std::endl;
        setPositionData(qobject_cast<sofaqtquick::bindings::SofaData*>(sofaObject->getData("translation")));
        m_oldTranslation = dynamic_cast<sofa::component::visualmodel::OglModel*>(sofaObject->rawBase())->m_translation.getValue();
    }

    emit sofaObjectChanged(sofaObject);
}


const QVector3D Manipulator::position() const {
    if (!myPosition || !myPosition->rawData()) QVector3D();

    const AbstractTypeInfo* typeinfo = myPosition->rawData()->getValueTypeInfo();
    const void* valueptr = myPosition->rawData()->getValueVoidPtr();

    return QVector3D(float(typeinfo->getScalarValue(valueptr, 0)),
                     float(typeinfo->getScalarValue(valueptr, 1)),
                     float(typeinfo->getScalarValue(valueptr, 2)));
}

sofaqtquick::bindings::SofaData* Manipulator::positionData()
{
    return myPosition;
}


void Manipulator::setPosition(const QVector3D& newPosition)
{
    if (!myPosition || !myPosition->rawData() || newPosition == position())
        return;

    const sofa::defaulttype::AbstractTypeInfo* typeinfo = myPosition->rawData()->getValueTypeInfo();
    void* valueptr = typeinfo->getValuePtr(myPosition->rawData()->beginEditVoidPtr());
    typeinfo->setScalarValue(valueptr, 0, double(newPosition.x()));
    typeinfo->setScalarValue(valueptr, 1, double(newPosition.y()));
    typeinfo->setScalarValue(valueptr, 2, double(newPosition.z()));
    myPosition->rawData()->endEditVoidPtr();
    emit positionChanged(position());
}

void Manipulator::setPositionData(SofaData* newPosition)
{
    if(newPosition == myPosition)
        return;

    myPosition = newPosition;
    emit positionChanged(position());
}

const QQuaternion Manipulator::orientation() const {
    if (!myOrientation || !myOrientation->rawData()) QQuaternion();

    const sofa::defaulttype::AbstractTypeInfo* typeinfo = myOrientation->rawData()->getValueTypeInfo();
    const void* valueptr = myOrientation->rawData()->getValueVoidPtr();

    return QQuaternion(float(typeinfo->getScalarValue(valueptr, 0)),
                       float(typeinfo->getScalarValue(valueptr, 1)),
                       float(typeinfo->getScalarValue(valueptr, 2)),
                       float(typeinfo->getScalarValue(valueptr, 3)));
}

const QVector3D Manipulator::eulerOrientation() const {
    return orientation().toEulerAngles();
}

sofaqtquick::bindings::SofaData* Manipulator::orientationData()
{
    return myOrientation;
}


void Manipulator::setOrientation(const QVector3D &newOrientation)
{
    QQuaternion quat;
    quat.fromEulerAngles(newOrientation);
    setOrientation(quat);
}

void Manipulator::setOrientation(const QQuaternion &newOrientation)
{
    if (!myOrientation || !myOrientation->rawData() || newOrientation == orientation())
        return;

    const sofa::defaulttype::AbstractTypeInfo* typeinfo = myOrientation->rawData()->getValueTypeInfo();
    void* valueptr = const_cast<void*>(typeinfo->getValuePtr(myOrientation->rawData()->beginEditVoidPtr()));
    if (typeinfo->size() == 4) {
        typeinfo->setScalarValue(valueptr, 0, double(newOrientation.x()));
        typeinfo->setScalarValue(valueptr, 1, double(newOrientation.y()));
        typeinfo->setScalarValue(valueptr, 2, double(newOrientation.z()));
        typeinfo->setScalarValue(valueptr, 3, double(newOrientation.scalar()));
    } else {
        typeinfo->setScalarValue(valueptr, 0, double(newOrientation.toEulerAngles().x()));
        typeinfo->setScalarValue(valueptr, 1, double(newOrientation.toEulerAngles().y()));
        typeinfo->setScalarValue(valueptr, 2, double(newOrientation.toEulerAngles().z()));
    }
    myOrientation->rawData()->endEditVoidPtr();
    emit orientationChanged(orientation());
}


void Manipulator::setOrientationData(SofaData* newOrientation)
{
    if(newOrientation == myOrientation)
        return;

    myOrientation = newOrientation;
}


const QVector3D Manipulator::scale() const {
    if (!myScale || !myScale->rawData()) QVector3D();

    const sofa::defaulttype::AbstractTypeInfo* typeinfo = myScale->rawData()->getValueTypeInfo();
    const void* valueptr = myScale->rawData()->getValueVoidPtr();

    return QVector3D(float(typeinfo->getScalarValue(valueptr, 0)),
                     float(typeinfo->getScalarValue(valueptr, 1)),
                     float(typeinfo->getScalarValue(valueptr, 2)));
}

sofaqtquick::bindings::SofaData* Manipulator::scaleData()
{
    return myScale;
}

void Manipulator::setScale(const QVector3D& newScale)
{
    if (!myScale || !myScale->rawData() || newScale == scale())
        return;

    const sofa::defaulttype::AbstractTypeInfo* typeinfo = myScale->rawData()->getValueTypeInfo();
    void* valueptr = const_cast<void*>(typeinfo->getValuePtr(myScale->rawData()->beginEditVoidPtr()));

    typeinfo->setScalarValue(valueptr, 0, double(newScale.x()));
    typeinfo->setScalarValue(valueptr, 1, double(newScale.y()));
    typeinfo->setScalarValue(valueptr, 2, double(newScale.z()));
    myScale->rawData()->endEditVoidPtr();
    emit scaleChanged(scale());
}


void Manipulator::setScaleData(SofaData* newScale)
{
    if(newScale == myScale)
        return;

    myScale = newScale;

    scaleChanged(scale());
}

QVector3D Manipulator::right() const
{
    return orientation().rotatedVector(QVector3D(1.0f, 0.0f, 0.0f));
}

QVector3D Manipulator::up() const
{
    return orientation().rotatedVector(QVector3D(0.0f, 1.0f, 0.0f));
}

QVector3D Manipulator::dir() const
{
    return orientation().rotatedVector(QVector3D(0.0f, 0.0f, 1.0f));
}

QVector3D Manipulator::applyModelToPoint(const QVector3D& point) const
{
    QMatrix4x4 model(orientation().toRotationMatrix());
    model.setColumn(3, QVector4D(position(), 1.0f));

    return model.map(point);
}

QVector3D Manipulator::applyModelToVector(const QVector3D& vector) const
{
    return QMatrix4x4(orientation().toRotationMatrix()).mapVector(vector);
}

void Manipulator::draw(const SofaViewer& /*viewer*/) const
{
    // a base manipulator has no visual representation
}

void Manipulator::pick(const SofaViewer& viewer) const
{
    draw(viewer);
}

void Manipulator::onValueChanged(const QVariant& /*newValue*/)
{
    if (mySofaObject && mySofaObject->rawBase())
    {
        BaseObject* o = mySofaObject->rawBase()->toBaseObject();
        if (o) {
            if (o->getClassName() == "MechanicalObject") {
                o->findData("position")->read(o->findData("reset_position")->getValueString());

                o->init();
                /// Yes... calling reinit / bwdInit after init() on a
                /// MechanicalObject applies the translation twice. no duh...
                //            o->reinit();
                //            o->bwdInit();
                std::cout << "position " << o->findData("position")->getValueString() << std::endl;
                std::cout << "translation " << o->findData("translation")->getValueString() << std::endl;
            }
            if (o->getClassName() == "OglModel") {
                auto vecData = dynamic_cast<sofa::Data<sofa::helper::vector<sofa::defaulttype::Vec3d>>*>(o->findData("position"));
                if (vecData)
                {
                    auto& vec = *vecData->beginEdit();
                    for (auto& v : vec)
                        v -= m_oldTranslation;
                    vecData->endEdit();
                    std::cout << "translation " << o->findData("translation")->getValueString() << std::endl;
                    o->init();
                    o->reinit();
                    o->bwdInit();
                    std::cout << "position " << o->findData("position")->getValueString() << std::endl;
                    std::cout << "translation " << o->findData("translation")->getValueString() << std::endl;
                }
            }
        }
    }
}

}  // namespace sofaqtquick
