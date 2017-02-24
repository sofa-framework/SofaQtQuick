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

#include "ImagePlaneView.h"
#include "ImagePlaneModel.h"

#include <image/ImageTypes.h>

#include <QPainter>
#include <qmath.h>

using cimg_library::CImg;

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;

ImagePlaneView::ImagePlaneView(QQuickItem* parent) : QQuickPaintedItem(parent),
    myImagePlaneModel(0),
    myAxis(0),
    myIndex(0),
    myImage(),
    myLength(0),
    myShowModels(true),
    myMinIntensity(0.0f),
    myMaxIntensity(1.0f)
{
    connect(this, &ImagePlaneView::imagePlaneModelChanged,  this, &ImagePlaneView::update);
    connect(this, &ImagePlaneView::axisChanged,             this, &ImagePlaneView::update);
    connect(this, &ImagePlaneView::indexChanged,            this, &ImagePlaneView::update);
    connect(this, &ImagePlaneView::showModelsChanged,       this, &ImagePlaneView::update);
    connect(this, &ImagePlaneView::minIntensityChanged,     this, &ImagePlaneView::update);
    connect(this, &ImagePlaneView::maxIntensityChanged,     this, &ImagePlaneView::update);
}

bool ImagePlaneView::containsPoint(const QVector3D& wsPoint) const
{
    bool result = false;
    if(!myImagePlaneModel || !myImagePlaneModel->imagePlane())
        return result;

    QVector3D isPoint = myImagePlaneModel->imagePlane()->toImagePoint(wsPoint);

    if(0 == myAxis) // x - zy
        result = (myIndex - 0.5f < isPoint.x() && isPoint.x() < myIndex + 0.5f);
    else if(1 == myAxis) // y - xz
        result = (myIndex - 0.5f < isPoint.y() && isPoint.y() < myIndex + 0.5f);
    else if(2 == myAxis) // z - xy
        result = (myIndex - 0.5f < isPoint.z() && isPoint.z() < myIndex + 0.5f);

    return result;
}

void ImagePlaneView::paint(QPainter* painter)
{
    QImage myImageScaled = myImage.scaled(width(), height(), Qt::AspectRatioMode::IgnoreAspectRatio);
    painter->drawImage(0, 0, myImageScaled);
}

QRgb ImagePlaneView::computePixelColor(int r, int g, int b) const
{
    const float minPixelIntensity = myMinIntensity * 255.0f;
    const float intensityRange = qMax(1.0f / 255.0f, myMaxIntensity - myMinIntensity);

    r = qFloor((r - minPixelIntensity) / intensityRange);
    g = qFloor((g - minPixelIntensity) / intensityRange);
    b = qFloor((b - minPixelIntensity) / intensityRange);

    return qRgb(qBound(0, r, 255),
                qBound(0, g, 255),
                qBound(0, b, 255));
}

void ImagePlaneView::update()
{
    if(!myImagePlaneModel || -1 == myIndex || myAxis < 0 || myAxis > 5)
    {
        myImage = QImage();
        return;
    }

    const cimg_library::CImg<unsigned char>& plane = myImagePlaneModel->retrieveSlice(myIndex, myAxis);
    if(plane.width() != myImage.width() || plane.height() != myImage.height())
    {
        myImage = QImage(plane.width(), plane.height(), QImage::Format_RGB32);
        QVector3D scale = myImagePlaneModel->imagePlane()->getScale();
        double rx = 1;
        double ry = 1;

        if (myAxis == 2) { rx = scale.x(); ry = scale.y(); }
        else if (myAxis == 1) { rx = scale.x(); ry = scale.z(); }
        else { rx = scale.z(); ry = scale.y(); }

        setImplicitWidth(plane.width());
        setImplicitHeight(plane.height() * (ry/rx));
    }

    if(1 == plane.spectrum())
    {
        for(int y = 0; y < myImage.height(); y++)
            for(int x = 0; x < myImage.width(); x++)
                myImage.setPixel(x, y, computePixelColor(plane(x, y, 0, 0), plane(x, y, 0, 0), plane(x, y, 0, 0)));
    }
    else if(2 == plane.spectrum())
    {
        for(int y = 0; y < myImage.height(); y++)
            for(int x = 0; x < myImage.width(); x++)
                myImage.setPixel(x, y, computePixelColor(plane(x, y, 0, 0), plane(x, y, 0, 1), plane(x, y, 0, 0)));
    }
    else
    {
        for(int y = 0; y < myImage.height(); y++)
            for(int x = 0; x < myImage.width(); x++)
                myImage.setPixel(x, y, computePixelColor(plane(x, y, 0, 0), plane(x, y, 0, 1), plane(x, y, 0, 2)));
    }

    if(myShowModels)
    {
        CImg<unsigned char> slicedModels = myImagePlaneModel->retrieveSlicedModels(myIndex, myAxis);
        if(slicedModels)
            for(int y = 0; y < myImage.height(); y++)
                for(int x = 0; x < myImage.width(); x++)
                    if(slicedModels(x, y, 0, 0) || slicedModels(x, y, 0, 1) || slicedModels(x, y, 0, 2))
                        myImage.setPixel(x, y, qRgb(slicedModels(x, y, 0, 0), slicedModels(x, y, 0, 1), slicedModels(x, y, 0, 2)));
    }

    QQuickItem::update();
}

void ImagePlaneView::setImagePlaneModel(ImagePlaneModel* imagePlaneModel)
{
    if(imagePlaneModel == myImagePlaneModel)
        return;

    myImagePlaneModel = imagePlaneModel;

    if(myImagePlaneModel)
    {
        myLength = myImagePlaneModel->length(myAxis);
        myIndex = myImagePlaneModel->currentIndex(myAxis);
    }

    imagePlaneModelChanged();

    if(myImagePlaneModel)
    {
        lengthChanged();
        indexChanged();
    }
}

void ImagePlaneView::setAxis(int axis)
{
    if(axis == myAxis)
        return;

    myAxis = axis;

    axisChanged();
}

void ImagePlaneView::setIndex(int index)
{
    if(index == myIndex)
        return;

    myIndex = index;

    indexChanged();
}

void ImagePlaneView::setLength(int length)
{
    if(length == myLength)
        return;

    myLength = length;

    lengthChanged();
}

void ImagePlaneView::setShowModels(bool showModels)
{
    if(showModels == myShowModels)
        return;

    myShowModels = showModels;

    showModelsChanged();
}

void ImagePlaneView::setMinIntensity(float minIntensity)
{
    if(minIntensity == myMinIntensity)
        return;

    myMinIntensity = minIntensity;

    minIntensityChanged();
}

void ImagePlaneView::setMaxIntensity(float maxIntensity)
{
    if(maxIntensity == myMaxIntensity)
        return;

    myMaxIntensity = maxIntensity;

    maxIntensityChanged();
}

}

}
