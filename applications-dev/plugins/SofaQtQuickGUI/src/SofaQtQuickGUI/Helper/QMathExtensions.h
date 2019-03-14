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
#include <QMatrix4x4>

namespace sofaqtquick::helper
{

class QMath : public QObject
{
    Q_OBJECT

public:
    template<class M>
    static const M& Zero() { static M m;
                             m.fill(0.0);
                                                  return m; }

    template<class M>
    static const M& Identity() { static M m;
                                 return m; }

    static void setMatrixFrom( QMatrix4x4& dest, const double* dmat );

    Q_INVOKABLE QQuaternion quaternionFromEulerAngles(const QVector3D& eulerAngles) const;
    Q_INVOKABLE QVector3D quaternionToEulerAngles(const QQuaternion& quaternion) const;
    Q_INVOKABLE QVariantList quaternionToAxisAngle(const QQuaternion& quaternion) const; // return [QVector3D axis, float angle];
    Q_INVOKABLE QQuaternion quaternionFromAxisAngle(const QVector3D& axis, float angle) const;
    Q_INVOKABLE QQuaternion quaternionDifference(const QQuaternion& q0, const QQuaternion& q1) const;
    Q_INVOKABLE QQuaternion quaternionMultiply(const QQuaternion& q0, const QQuaternion& q1) const;
    Q_INVOKABLE QQuaternion quaternionConjugate(const QQuaternion& q) const;
    Q_INVOKABLE QVector3D quaternionRotate(const QQuaternion& q, const QVector3D& v) const;
    Q_INVOKABLE QVariantList quaternionSwingTwistDecomposition(const QQuaternion& q, const QVector3D& direction);
};

}
