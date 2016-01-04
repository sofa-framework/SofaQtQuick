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
    myLength(0)
{
    connect(this, &ImagePlaneView::imagePlaneModelChanged,  this, &ImagePlaneView::update);
    connect(this, &ImagePlaneView::axisChanged,             this, &ImagePlaneView::update);
    connect(this, &ImagePlaneView::indexChanged,            this, &ImagePlaneView::update);
}

void ImagePlaneView::paint(QPainter* painter)
{
    QSize size(myImage.size());
    size.scale(width(), height(), Qt::AspectRatioMode::KeepAspectRatio);

    double scaleRatio = 1.0;
    if(qFloor(width()) == size.width())
        scaleRatio = width() / myImage.width();
    else
        scaleRatio = height() / myImage.height();

    painter->translate((width()  - size.width() ) * 0.5,
                       (height() - size.height()) * 0.5);

    painter->scale(scaleRatio, scaleRatio);

    painter->drawImage(0, 0, myImage);
}

void ImagePlaneView::update()
{
    if(!myImagePlaneModel || -1 == myIndex || myAxis < 0 || myAxis > 5)
    {
        myImage = QImage();
        return;
    }

    setLength(myImagePlaneModel->length(myAxis));

    const cimg_library::CImg<unsigned char>& slice = myImagePlaneModel->retrieveSlice(myIndex, myAxis);
    if(slice.width() != myImage.width() || slice.height() != myImage.height())
    {
        myImage = QImage(slice.width(), slice.height(), QImage::Format_RGB32);
        setImplicitWidth(slice.width());
        setImplicitHeight(slice.height());
    }

    if(1 == slice.spectrum())
    {
        for(int y = 0; y < myImage.height(); y++)
            for(int x = 0; x < myImage.width(); x++)
                myImage.setPixel(x, y, qRgb(slice(x, y, 0, 0), slice(x, y, 0, 0), slice(x, y, 0, 0)));
    }
    else if(2 == slice.spectrum())
    {
        for(int y = 0; y < myImage.height(); y++)
            for(int x = 0; x < myImage.width(); x++)
                myImage.setPixel(x, y, qRgb(slice(x, y, 0, 0), slice(x, y, 0, 1), slice(x, y, 0, 0)));
    }
    else
    {
        for(int y = 0; y < myImage.height(); y++)
            for(int x = 0; x < myImage.width(); x++)
                myImage.setPixel(x, y, qRgb(slice(x, y, 0, 0), slice(x, y, 0, 1), slice(x, y, 0, 2)));
    }

    CImg<unsigned char> slicedModels = myImagePlaneModel->retrieveSlicedModels(myIndex, myAxis);
    if(slicedModels)
        for(int y = 0; y < myImage.height(); y++)
            for(int x = 0; x < myImage.width(); x++)
                if(slicedModels(x, y, 0, 0) || slicedModels(x, y, 0, 1) || slicedModels(x, y, 0, 2))
                    myImage.setPixel(x, y, qRgb(slicedModels(x, y, 0, 0), slicedModels(x, y, 0, 1), slicedModels(x, y, 0, 2)));

    // display selected pixels on the image plane
//    for(int i = 0; i < points.size(); ++i)
//    {
//        Coord point = points[i];
//        if(myAxis==0 && myIndex == point.x())
//            myImage.setPixel(QPoint(point.z(), point.y()), qRgb(255, 0, 0));

//        else if(myAxis==1 && myIndex == point.y())
//            myImage.setPixel(QPoint(point.x(), point.z()), qRgb(255, 0, 0));

//        else if (myAxis==2 && myIndex == point.z())
//            myImage.setPixel(QPoint(point.x(), point.y()), qRgb(255, 0, 0));
//    }

    QQuickItem::update();
}

void ImagePlaneView::setImagePlaneModel(ImagePlaneModel* imagePlaneModel)
{
    if(imagePlaneModel == myImagePlaneModel)
        return;

    myImagePlaneModel = imagePlaneModel;

    imagePlaneModelChanged();
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

}

}
