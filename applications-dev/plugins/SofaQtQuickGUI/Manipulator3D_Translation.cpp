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

#include "Manipulator3D_Translation.h"
#include "SofaViewer.h"

#include <QApplication>
#include <GL/glew.h>
#include <QMatrix4x4>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

Manipulator3D_Translation::Manipulator3D_Translation(QObject* parent) : Manipulator(parent),
    myAxis("xyz")
{

}

Manipulator3D_Translation::~Manipulator3D_Translation()
{

}

void Manipulator3D_Translation::setAxis(QString newAxis)
{
    if(newAxis == myAxis)
        return;

    myAxis = newAxis;

    axisChanged(newAxis);
}

void Manipulator3D_Translation::draw(const SofaViewer& viewer) const
{
    internalDraw(viewer, false);
}

void Manipulator3D_Translation::pick(const SofaViewer& viewer) const
{
    internalDraw(viewer, true);
}

void Manipulator3D_Translation::internalDraw(const SofaViewer& viewer, bool isPicking) const
{
    if(!visible())
        return;

    Camera* camera = viewer.camera();
    if(!camera)
        return;

    bool xAxis = (-1 != myAxis.indexOf('x'));
    bool yAxis = (-1 != myAxis.indexOf('y'));
    bool zAxis = (-1 != myAxis.indexOf('z'));
    int axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0) + (zAxis ? 1 : 0);

    if(0 == axisNum || 3 == axisNum)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(position().x(), position().y(), position().z());

    QVector3D axis = QVector3D(0.0, 0.0, 1.0);
    if(xAxis)
        axis = QVector3D(1.0, 0.0, 0.0);
    else if(yAxis)
        axis = QVector3D(0.0, 1.0, 0.0);

    // object
    float height = 0.2f;
    {
        QVector4D p0 = camera->projection() * camera->view() * QVector4D(position(), 1.0);
        QVector4D p1 = camera->projection() * camera->view() * QVector4D(position() + camera->up(), 1.0);
        QVector3D direction = ((p1 / p1.w() - p0 / p0.w()).toVector3D());

        height *= 1.0 / direction.length();
    }

    float width = height * 0.05f;
    if(isPicking)
        width *= 2.5f;

    glDisable(GL_CULL_FACE);

    glLineWidth(width);

    QColor color(xAxis ? 255 : 0, yAxis ? 255 : 0, zAxis ? 255 : 0);
    glColor3f(color.redF(), color.greenF(), color.blueF());

    glEnable(GL_POLYGON_OFFSET_FILL);

    // draw arrows
    if(1 == axisNum)
    {
        width *= 0.5f;

        if(xAxis || yAxis)
            glRotated(-90.0, 1.0, 0.0, 0.0);

        if(xAxis)
            glRotated(90.0, 0.0, 1.0, 0.0);

        glPolygonOffset(-1.0f, -1.0f);

        glBegin(GL_QUADS);
        {
            glVertex3f(-0.2 * height,   0.0,    0.8 * height + width);
            glVertex3f(-0.2 * height,   0.0,    0.8 * height - width);
            glVertex3f( 0.0,            0.0,          height - width);
            glVertex3f( 0.0,            0.0,          height + width);

            glVertex3f( 0.0,            0.0,          height + width);
            glVertex3f( 0.0,            0.0,          height - width);
            glVertex3f( 0.2 * height,   0.0,    0.8 * height - width);
            glVertex3f( 0.2 * height,   0.0,    0.8 * height + width);

            glVertex3f(       -width,   0.0,          height - width);
            glVertex3f(       -width,   0.0,                   width);
            glVertex3f(        width,   0.0,                   width);
            glVertex3f(        width,   0.0,          height - width);
        }
        glEnd();
    }
    else // draw quad surrounded by lines
    {
        height *= 0.33f;
        if(isPicking)
            height *= 1.25f;

        if(!xAxis)
            glRotated(-90.0, 0.0, 1.0, 0.0);
        else if(!yAxis)
            glRotated(90.0, 1.0, 0.0, 0.0);

        glPolygonOffset(-1.0f, -3.0f);

        glBegin(GL_QUADS);
        {
            glVertex3f(   0.0, height, 0.0);
            glVertex3f(   0.0,    0.0, 0.0);
            glVertex3f(height,    0.0, 0.0);
            glVertex3f(height, height, 0.0);
        }
        glEnd();
    }

    glDisable(GL_POLYGON_OFFSET_FILL);

    glLineWidth(1.0f);

    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}
