#pragma once

#include <sofa/defaulttype/Vec3Types.h>
#include <sofa/defaulttype/Quat.h>
#include <QVector3D>
#include <QQuaternion>

namespace sofaqtquick::helper
{

QVector3D toQVector3D(const sofa::defaulttype::Vec3d& v);

 sofa::defaulttype::Vec3d toVec3d(const QVector3D& v);

  QVector4D toQVector4D(const sofa::defaulttype::Vec4d& v);

  sofa::defaulttype::Vec4d toVec4d(const QVector4D& v);
  QQuaternion toQQuaternion(const sofa::defaulttype::Quaternion& q);
  sofa::defaulttype::Quaternion toQuaternion(const QQuaternion& q);
  float toRadians(float degrees);
  float toDegrees(float radians);
  double toRadians(double degrees);
  double toDegrees(double radians);
  QVector4D toAxisAngle(const QQuaternion& q);

}  // namespace sofaqtquick::helper

