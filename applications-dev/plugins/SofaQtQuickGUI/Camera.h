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

#ifndef CAMERA_H
#define CAMERA_H

#include "SofaQtQuickGUI.h"
#include <QObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QPoint>

namespace sofa
{

namespace qtquick
{

/// \class Used in a SofaViewer to locate the user point of view
class SOFA_SOFAQTQUICKGUI_API Camera : public QObject
{
    Q_OBJECT

public:
	explicit Camera(QObject* parent = 0);
	~Camera();

public:
    Q_PROPERTY(bool orthographic READ orthographic WRITE setOrthographic NOTIFY orthographicChanged)
    Q_PROPERTY(QVector3D target READ target WRITE setTarget NOTIFY targetChanged)

public:
    bool orthographic() const {return myOrthographic;}
    void setOrthographic(bool orthographic);

    QVector3D target() const {return myTarget;}
    void setTarget(const QVector3D& target);

signals:
    void orthographicChanged();
    void targetChanged();

public:
	const QMatrix4x4& projection() const;
	const QMatrix4x4& view() const;
	const QMatrix4x4& model() const;

    Q_INVOKABLE QVector3D eye() const				{return  model().column(3).toVector3D();}

    Q_INVOKABLE QVector3D direction() const			{return -model().column(2).toVector3D().normalized();}
    Q_INVOKABLE QVector3D up() const				{return  model().column(1).toVector3D().normalized();}
    Q_INVOKABLE QVector3D right() const				{return  model().column(0).toVector3D().normalized();}

    Q_INVOKABLE double zNear() const                {return myZNear;}
    Q_INVOKABLE double zFar() const                 {return myZFar;}

    Q_INVOKABLE double aspectRatio() const          {return myAspectRatio;}

    /// @return depth in normalized device coordinate (ndc) space
    Q_INVOKABLE double computeDepth(const QVector3D& wsPosition) const;

    Q_INVOKABLE QVector3D projectOnViewPlane(const QVector3D& wsPosition, double depth) const;

    Q_INVOKABLE QVector3D projectOnViewSpaceXAxis(const QVector3D& wsVector) const;
    Q_INVOKABLE QVector3D projectOnViewSpaceYAxis(const QVector3D& wsVector) const;

public slots:
    void viewFromFront();
    void viewFromBack();
    void viewFromLeft();
    void viewFromRight();
    void viewFromTop();
    void viewFromBottom();
    void viewIsometric();

    void move(double x, double y, double z);
    void turn(double angleAroundX, double angleAroundY, double angleAroundZ);
    void zoom(double factor);

    void lookAt(const QVector3D& eye, const QVector3D& target, const QVector3D& up);
    void fit(const QVector3D& min, const QVector3D& max);

    void alignCameraAxis();

public:
    void setOrthoLeft(double left);
    void setOrthoRight(double right);
    void setOrthoBottom(double bottom);
    void setOrthoTop(double top);

    void setPerspectiveFovY(double fovY);
    void setAspectRatio(double aspectRatio);
	void setZNear(double zNear);
	void setZFar(double zFar);

private:
    void computeOrthographic();
    void computeModel();
    QVector3D computeNearestAxis(QVector3D axis,int& nearAxisIndex, int caseTested = -1);

private:
    bool                myOrthographic;
    double              myOrthoLeft;
    double              myOrthoRight;
    double              myOrthoBottom;
    double              myOrthoTop;
    double				myPerspectiveFovY;
    double				myAspectRatio;
	double				myZNear;
	double				myZFar;

	mutable QMatrix4x4	myProjection;
    mutable QMatrix4x4	myView;
    QMatrix4x4          myModel;

    QVector3D           myTarget;

	mutable bool		myProjectionDirty;
    mutable bool		myViewDirty;
};

}

}

#endif // CAMERA_H
