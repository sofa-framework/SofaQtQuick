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

    virtual int currentIndex(int axis) const = 0;
    virtual void setCurrentIndex(int index, int axis) = 0;
    virtual cimg_library::CImg<unsigned char> retrieveSlice(int index, int axis) const = 0;
    virtual cimg_library::CImg<unsigned char> retrieveSlicedModels(int index, int axis) const = 0;
    virtual int length(int axis) const = 0;
    virtual QVector3D toImagePoint(const QVector3D& wsPoint) const = 0;
    virtual QVector3D toWorldPoint(const QVector3D& isPoint) const = 0;

};

template<class T>
class ImagePlaneWrapper : public BaseImagePlaneWrapper
{
public:
    ImagePlaneWrapper(sofa::defaulttype::ImagePlane<T>& imagePlane) : BaseImagePlaneWrapper(),
        myImagePlane(imagePlane),
        myAxisLengths()
    {
        int axisCount = sofa::defaulttype::ImagePlane<T>::pCoord::spatial_dimensions;
        for(int axis = 0; axis < axisCount; ++axis)
            myAxisLengths.append(myImagePlane.getDimensions()[axis]);
    }

    int currentIndex(int axis) const
    {
        if(axis < 0 || axis >= sofa::defaulttype::ImagePlane<T>::Coord::total_size)
        {
            msg_error("image_qtquickgui") << "Inconsistent axis";
            return -1;
        }

        return myImagePlane.getPlane()[axis];
    }

    void setCurrentIndex(int axis, int index)
    {
        typename sofa::defaulttype::ImagePlane<T>::Coord coord = myImagePlane.getPlane();

        if(axis < 0 || axis >= sofa::defaulttype::ImagePlane<T>::Coord::total_size)
        {
            msg_error("image_qtquickgui") << "Inconsistent axis";
            return;
        }

        coord[axis] = index;

        myImagePlane.setPlane(coord);
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
        return myAxisLengths[axis];
    }

    QVector3D toImagePoint(const QVector3D& wsPoint) const
    {
        typedef typename sofa::defaulttype::ImagePlane<T>::Coord Coord;
        Coord result = myImagePlane.get_pointImageCoord(Coord(wsPoint.x(), wsPoint.y(), wsPoint.z()));

        return QVector3D(result.x(), result.y(), result.z());
    }

    QVector3D toWorldPoint(const QVector3D& isPoint) const
    {
        typedef typename sofa::defaulttype::ImagePlane<T>::Coord Coord;
        Coord result = myImagePlane.get_pointCoord(Coord(isPoint.x(), isPoint.y(), isPoint.z()));

        return QVector3D(result.x(), result.y(), result.z());
    }

private:
    sofa::defaulttype::ImagePlane<T>& myImagePlane;
    QVector<int> myAxisLengths;

};

class SOFA_IMAGE_QTQUICKGUI_API ImagePlaneModel : public QObject
{
    Q_OBJECT

public:
    ImagePlaneModel(QObject* parent = 0);

public:
    Q_PROPERTY(sofa::qtquick::SofaData* sofaData READ sofaData WRITE setSofaData NOTIFY sofaDataChanged)

public:
    SofaData* sofaData() const;
    void setSofaData(sofa::qtquick::SofaData* sofaData);

public:
    BaseImagePlaneWrapper* imagePlane() const;
    void setImagePlane(BaseImagePlaneWrapper* imagePlane) {myImagePlane = imagePlane;}

    Q_INVOKABLE int currentIndex(int axis) const;
    Q_INVOKABLE void setCurrentIndex(int axis, int index);

    Q_INVOKABLE int length(int axis) const;

    Q_INVOKABLE QPointF toImagePoint(int axis, const QVector3D& wsPoint) const;
    Q_INVOKABLE QVector3D toWorldPoint(int axis, int index, const QPointF& isPoint) const;

    cimg_library::CImg<unsigned char> retrieveSlice(int index, int axis) const;
    cimg_library::CImg<unsigned char> retrieveSlicedModels(int index, int axis) const;

signals:
    void sofaDataChanged();
    void currentIndexChanged();

private slots:
    void handleSofaDataChange();

private:
    sofa::qtquick::SofaData*        mySofaData;
    mutable BaseImagePlaneWrapper*  myImagePlane;

};

}

}

#endif // SOFA_IMAGE_QTQUICKGUI_IMAGEPLANEMODEL_H
