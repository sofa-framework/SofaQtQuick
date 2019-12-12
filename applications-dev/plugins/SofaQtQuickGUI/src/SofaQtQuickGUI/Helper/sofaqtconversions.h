#pragma once

#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/defaulttype/Quat.h>
#include <QVector3D>
#include <QQuaternion>

namespace sofaqtquick::helper
{

QVector3D toQVector3D(const sofa::defaulttype::Vec3d& v)
{
    return QVector3D(float(v.x()), float(v.y()), float(v.z()));
}

sofa::defaulttype::Vec3d toVec3d(const QVector3D& v)
{
    return sofa::defaulttype::Vec3d(double(v.x()), double(v.y()), double(v.z()));
}

QVector4D toQVector4D(const sofa::defaulttype::Vec4d& v)
{
    return QVector4D(float(v.x()), float(v.y()), float(v.z()), float(v.w()));
}

sofa::defaulttype::Vec4d toVec4d(const QVector4D& v)
{
    return sofa::defaulttype::Vec4d(double(v.x()), double(v.y()), double(v.z()), double(v.w()));
}

QQuaternion toQQuaternion(const sofa::defaulttype::Quaternion& q)
{
    return QQuaternion(float(q[3]), float(q[0]), float(q[1]), float(q[2]));
}

sofa::defaulttype::Quaternion toQuaternion(const QQuaternion& q)
{
    return sofa::defaulttype::Quaternion(double(q.x()), double(q.y()), double(q.z()), double(q.scalar()));
}

float toRadians(float degrees)
{
    return float(double(degrees) * M_PI / 180.0);
}

float toDegrees(float radians)
{
    return float(double(radians) * 180.0 / M_PI);
}

double toRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

double toDegrees(double radians)
{
    return radians * 180.0 / M_PI;
}

QVector4D toAxisAngle(const QQuaternion& q)
{
    float angle = 2 * std::acos(q.scalar());
    float x = q.x() / std::sqrt(1-q.scalar()*q.scalar());
    float y = q.y() / std::sqrt(1-q.scalar()*q.scalar());
    float z = q.z() / std::sqrt(1-q.scalar()*q.scalar());
    return QVector4D(toDegrees(x), toDegrees(y), toDegrees(z), toDegrees(angle));
}




}  // namespace sofaqtquick::helper

