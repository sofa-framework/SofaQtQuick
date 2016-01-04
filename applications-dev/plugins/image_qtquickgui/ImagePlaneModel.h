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

#ifndef SOFA_IMAGE_QTQUICKGUI_IMAGEPLANEMODEL_H
#define SOFA_IMAGE_QTQUICKGUI_IMAGEPLANEMODEL_H

#include "ImageQtQuickGUI.h"

#include <SofaQtQuickGUI/SofaScene.h>
#include <image/ImageTypes.h>

namespace sofa
{

namespace qtquick
{

class BaseImagePlaneWrapper
{
public:
    virtual ~BaseImagePlaneWrapper()
    {

    }

    virtual cimg_library::CImg<unsigned char> retrieveSlice(int index, int axis) const = 0;
    virtual cimg_library::CImg<unsigned char> retrieveSlicedModels(int index, int axis) const = 0;
    virtual int length(int axis) const = 0;

};

template<class T>
class ImagePlaneWrapper : public BaseImagePlaneWrapper
{
public:
    ImagePlaneWrapper(const sofa::defaulttype::ImagePlane<T>& imagePlane) : BaseImagePlaneWrapper(),
        myImagePlane(imagePlane)
    {

    }

    cimg_library::CImg<unsigned char> retrieveSlice(int index, int axis) const
    {
        return convertToUC(myImagePlane.get_slice((unsigned int) index, (unsigned int) axis));
    }

    cimg_library::CImg<unsigned char> retrieveSlicedModels(int index, int axis) const
    {
        return convertToUC(myImagePlane.get_slicedModels((unsigned int) index, (unsigned int) axis));
    }

    int length(int axis) const
    {
        return myImagePlane.getDimensions()[axis];
    }

private:
    const sofa::defaulttype::ImagePlane<T>&     myImagePlane;

};

class SOFA_IMAGE_QTQUICKGUI_API ImagePlaneModel : public QObject
{
    Q_OBJECT

public:
    ImagePlaneModel(QObject* parent = 0);

public:
    cimg_library::CImg<unsigned char> retrieveSlice(int index, int axis) const;
    cimg_library::CImg<unsigned char> retrieveSlicedModels(int index, int axis) const;
    int length(int axis) const;

public:
    Q_PROPERTY(sofa::qtquick::SofaData* sofaData READ sofaData WRITE setSofaData NOTIFY sofaDataChanged)

public:
    SofaData* sofaData() const                {return mySofaData;}
    BaseImagePlaneWrapper* imagePlane() const;

protected:
    void setSofaData(sofa::qtquick::SofaData* sofaData);
    void setImagePlane(BaseImagePlaneWrapper* imagePlane);

signals:
    void sofaDataChanged();
    void imagePlaneChanged();

private slots:
    void handleSofaDataChange();

private:
    sofa::qtquick::SofaData*        mySofaData;
    mutable BaseImagePlaneWrapper*  myImagePlane;

};

}

}

#endif // SOFA_IMAGE_QTQUICKGUI_IMAGEPLANEMODEL_H
