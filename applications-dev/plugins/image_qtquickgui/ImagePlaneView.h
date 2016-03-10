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

#ifndef SOFA_IMAGE_QTQUICKGUI_IMAGEPLANEVIEW_H
#define SOFA_IMAGE_QTQUICKGUI_IMAGEPLANEVIEW_H

#include "ImageQtQuickGUI.h"

#include <QQuickPaintedItem>
#include <QImage>

namespace sofa
{

namespace qtquick
{

class ImagePlaneModel;

class SOFA_IMAGE_QTQUICKGUI_API ImagePlaneView : public QQuickPaintedItem
{
    Q_OBJECT

public:
    ImagePlaneView(QQuickItem* parent = 0);

public:
    void paint(QPainter* painter);

public slots:
    void update();

public:
    Q_PROPERTY(sofa::qtquick::ImagePlaneModel* imagePlaneModel READ imagePlaneModel WRITE setImagePlaneModel NOTIFY imagePlaneModelChanged)
    Q_PROPERTY(int axis READ axis WRITE setAxis NOTIFY axisChanged)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(int length READ length NOTIFY lengthChanged)
//    Q_PROPERTY(double paintedWidth READ paintedWidth NOTIFY paintedWidthChanged)
//    Q_PROPERTY(double paintedHeight READ paintedHeight NOTIFY paintedHeightChanged)

public:
    sofa::qtquick::ImagePlaneModel* imagePlaneModel() const {return myImagePlaneModel;}
    int axis() const {return myAxis;}
    int index() const {return myIndex;}
    int length() const {return myLength;}
//    int paintedWidth() const {return myPaintedWidth;}
//    int paintedHeight() const {return myPaintedHeight;}

public:
    Q_INVOKABLE bool containsPoint(const QVector3D& wsPoint) const;

protected:
    void setImagePlaneModel(sofa::qtquick::ImagePlaneModel* imagePlaneModel);
    void setAxis(int axis);
    void setIndex(int index);
    void setLength(int length);
//    void setPaintedWidth(double paintedWidth);
//    void setPaintedHeight(double paintedHeight);

protected:
//    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

signals:
    void imagePlaneModelChanged();
    void lengthChanged();
    void axisChanged();
    void indexChanged();
//    void paintedWidthChanged();
//    void paintedHeightChanged();

private:
    ImagePlaneModel*                    myImagePlaneModel;

    int                                 myAxis;
    int                                 myIndex;
    QImage                              myImage;
    int                                 myLength;
//    double                              myPaintedWidth;
//    double                              myPaintedHeight;

};

}

}

#endif // SOFA_IMAGE_QTQUICKGUI_IMAGEPLANEVIEW_H
