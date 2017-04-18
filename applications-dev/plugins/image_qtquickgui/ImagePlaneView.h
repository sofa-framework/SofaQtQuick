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

#include <image/ImageTypes.h>

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
    Q_PROPERTY(bool showModels READ showModels WRITE setShowModels NOTIFY showModelsChanged)

    Q_PROPERTY(float minIntensity READ minIntensity WRITE setMinIntensity NOTIFY minIntensityChanged)
    Q_PROPERTY(float maxIntensity READ maxIntensity WRITE setMaxIntensity NOTIFY maxIntensityChanged)

public:
    sofa::qtquick::ImagePlaneModel* imagePlaneModel() const {return myImagePlaneModel;}
    int axis() const {return myAxis;}
    int index() const {return myIndex;}
    int length() const {return myLength;}
    bool showModels() const {return myShowModels;}

    float minIntensity() const {return myMinIntensity;}
    float maxIntensity() const {return myMaxIntensity;}

public:
    Q_INVOKABLE bool containsPoint(const QVector3D& wsPoint) const;

protected:
    void setImagePlaneModel(sofa::qtquick::ImagePlaneModel* imagePlaneModel);
    void setAxis(int axis);
    void setIndex(int index);
    void setLength(int length);
    void setShowModels(bool showModels);

    void setMinIntensity(float minIntensity);
    void setMaxIntensity(float maxIntensity);

signals:
    void imagePlaneModelChanged();
    void lengthChanged();
    void axisChanged();
    void indexChanged();
    void showModelsChanged();

    void minIntensityChanged();
    void maxIntensityChanged();

private:
    QRgb computePixelColor(int r, int g, int b) const;

private:
    ImagePlaneModel*                    myImagePlaneModel;

    int                                 myAxis;
    int                                 myIndex;
    cimg_library::CImg<unsigned char>   myCImg;
    QImage                              myImage;
    int                                 myLength;
    bool                                myShowModels;

    float                               myMinIntensity;
    float                               myMaxIntensity;

};

}

}

#endif // SOFA_IMAGE_QTQUICKGUI_IMAGEPLANEVIEW_H
