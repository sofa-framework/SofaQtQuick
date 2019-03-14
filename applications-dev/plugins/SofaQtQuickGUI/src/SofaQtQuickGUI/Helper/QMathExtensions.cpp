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
#include <SofaQtQuickGUI/Helper/QMathExtensions.h>

namespace sofaqtquick::helper
{

void QMath::setMatrixFrom( QMatrix4x4& dest, const double* dmat )
{
    QMatrix4x4 tmp(dmat[0], dmat[1], dmat[2], dmat[3]
            , dmat[4], dmat[5], dmat[6], dmat[7]
            , dmat[8], dmat[9], dmat[10], dmat[11]
            , dmat[12], dmat[13], dmat[14], dmat[15]);

    dest = tmp;
}

QQuaternion QMath::quaternionFromEulerAngles(const QVector3D& eulerAngles) const
{
    return QQuaternion::fromEulerAngles(eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
}

QVector3D QMath::quaternionToEulerAngles(const QQuaternion& quaternion) const
{
    return quaternion.toEulerAngles();
}

QVariantList QMath::quaternionToAxisAngle(const QQuaternion& quaternion) const // return [QVector3D axis, float angle];
{
    QVector3D axis;
    float angle = 0.0f;

    quaternion.getAxisAndAngle(&axis, &angle);

    return QVariantList() << QVariant::fromValue(axis) << QVariant::fromValue(angle);
}

QQuaternion QMath::quaternionFromAxisAngle(const QVector3D& axis, float angle) const
{
    return QQuaternion::fromAxisAndAngle(axis, angle);
}

QQuaternion QMath::quaternionDifference(const QQuaternion& q0, const QQuaternion& q1) const
{
    return q1 * q0.conjugated();
}

QQuaternion QMath::quaternionMultiply(const QQuaternion& q0, const QQuaternion& q1) const
{
    return q0 * q1;
}

QQuaternion QMath::quaternionConjugate(const QQuaternion& q) const
{
    return q.conjugated();
}

QVector3D QMath::quaternionRotate(const QQuaternion& q, const QVector3D& v) const
{
    return q.rotatedVector(v);
}

/**
   Decompose the q rotation on to 2 parts.
   1. Twist - rotation around the "direction" vector
   2. Swing - rotation around axis that is perpendicular to "direction" vector
   The rotation can be composed back by
   rotation = swing * twist

   has singularity in case of swing_rotation close to 180 degrees rotation.
   if the input quaternion is of non-unit length, the outputs are non-unit as well
   otherwise, outputs are both unit
*/
QVariantList QMath::quaternionSwingTwistDecomposition(const QQuaternion& q, const QVector3D& direction)
{
    QVector3D ra(q.x(), q.y(), q.z()); // rotation axis
    QVector3D p = direction.normalized() * QVector3D::dotProduct(ra, direction.normalized()); // projection( ra, direction ); // return projection v1 on to v2  (parallel component)

    QQuaternion twist(q.scalar(), p.x(), p.y(), p.z());
    twist.normalize();

    QQuaternion swing = q * twist.conjugated();

    return QVariantList() << QVariant::fromValue(swing) << QVariant::fromValue(twist);
}


} /// sofaqtquick::helper

