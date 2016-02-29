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

#include <QDebug>
#include "ImagePlaneModel.h"

#include <sofa/core/objectmodel/BaseData.h>

using cimg_library::CImg;

namespace sofa
{

namespace qtquick
{

using namespace sofa::defaulttype;

ImagePlaneModel::ImagePlaneModel(QObject* parent) : QObject(parent),
    mySofaData(0),
    myImagePlane(0)
{
    connect(this, &ImagePlaneModel::sofaDataChanged, this, &ImagePlaneModel::handleSofaDataChange);
}

cimg_library::CImg<unsigned char> ImagePlaneModel::retrieveSlice(int index, int axis) const
{
    if(!imagePlane())
        return cimg_library::CImg<unsigned char>();

    return myImagePlane->retrieveSlice(index, axis);
}

cimg_library::CImg<unsigned char> ImagePlaneModel::retrieveSlicedModels(int index, int axis) const
{
    if(!imagePlane())
        return cimg_library::CImg<unsigned char>();

    return myImagePlane->retrieveSlicedModels(index, axis);
}

int ImagePlaneModel::length(int axis) const
{
    if(!imagePlane() || axis < 0 || axis > 5)
        return 0;

    return myImagePlane->length(axis);
}

BaseImagePlaneWrapper* ImagePlaneModel::imagePlane() const
{
    const core::objectmodel::BaseData* data = mySofaData->data();
    if(!data)
        myImagePlane = 0;

    return myImagePlane;
}

void ImagePlaneModel::setSofaData(SofaData* sofaData)
{
    if(sofaData == mySofaData)
        return;

    mySofaData = sofaData;

    sofaDataChanged();
}

void ImagePlaneModel::setImagePlane(BaseImagePlaneWrapper *imagePlane)
{
    if(imagePlane == myImagePlane)
        return;

    myImagePlane = imagePlane;

    imagePlaneChanged();
}

void ImagePlaneModel::handleSofaDataChange()
{
    delete myImagePlane;
    setImagePlane(0);

    if(!mySofaData)
        return;

    const core::objectmodel::BaseData* data = mySofaData->data();
    if(!data)
        return;

    QString type = QString::fromStdString(data->getValueTypeString());

    if(0 == type.compare("ImagePlane<char>"))
        setImagePlane(new ImagePlaneWrapper<char>(*(ImagePlane<char>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<unsigned char>"))
        setImagePlane(new ImagePlaneWrapper<unsigned char>(*(ImagePlane<unsigned char>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<int>"))
        setImagePlane(new ImagePlaneWrapper<int>(*(ImagePlane<int>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<unsigned int>"))
        setImagePlane(new ImagePlaneWrapper<unsigned int>(*(ImagePlane<unsigned int>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<short>"))
        setImagePlane(new ImagePlaneWrapper<short>(*(ImagePlane<short>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<unsigned short>"))
        setImagePlane(new ImagePlaneWrapper<unsigned short>(*(ImagePlane<unsigned short>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<long>"))
        setImagePlane(new ImagePlaneWrapper<long>(*(ImagePlane<long>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<unsigned long>"))
        setImagePlane(new ImagePlaneWrapper<unsigned long>(*(ImagePlane<unsigned long>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<float>"))
        setImagePlane(new ImagePlaneWrapper<float>(*(ImagePlane<float>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<double>"))
        setImagePlane(new ImagePlaneWrapper<double>(*(ImagePlane<double>*) data->getValueVoidPtr()));
    else if(0 == type.compare("ImagePlane<bool>"))
        setImagePlane(new ImagePlaneWrapper<bool>(*(ImagePlane<bool>*) data->getValueVoidPtr()));
    else
        msg_error("image_qtquickgui") << "Type unknown";
}

}

}
