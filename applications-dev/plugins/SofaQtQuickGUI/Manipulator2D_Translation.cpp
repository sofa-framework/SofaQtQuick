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

#include "Manipulator2D_Translation.h"
#include "SofaViewer.h"

#include <QApplication>
#include <GL/glew.h>
#include <QMatrix4x4>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

Manipulator2D_Translation::Manipulator2D_Translation(QObject* parent) : Manipulator(parent),
    myAxis("xy")
{

}

Manipulator2D_Translation::~Manipulator2D_Translation()
{

}

void Manipulator2D_Translation::setAxis(QString newAxis)
{
    if(newAxis == myAxis)
        return;

    myAxis = newAxis;

    axisChanged(newAxis);
}

void Manipulator2D_Translation::draw(const SofaViewer& viewer) const
{
    internalDraw(viewer, false);
}

void Manipulator2D_Translation::pick(const SofaViewer& viewer) const
{
    internalDraw(viewer, true);
}

void Manipulator2D_Translation::internalDraw(const SofaViewer& viewer, bool isPicking) const
{
    if(!visible())
        return;

    Camera* camera = viewer.camera();
    if(!camera)
        return;

    bool xAxis = (-1 != myAxis.indexOf('x'));
    bool yAxis = (-1 != myAxis.indexOf('y'));

    int axisNum = (xAxis ? 1 : 0) + (yAxis ? 1 : 0);

    if(0 == axisNum)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    QVector4D position = camera->projection() * camera->view() * QVector4D(Manipulator::position(), 1.0);
    position /= position.w();

    glTranslatef(position.x(), position.y(), position.z());
    glScalef(viewer.height() / viewer.width(), 1.0f, 1.0f);

    // object
    float height = 0.125f;

    float width = height * 0.1f;
    if(isPicking)
        width *= 2.5f;

    QColor color(xAxis ? 255 : 0, yAxis ? 255 : 0, (xAxis && yAxis) ? 255 : 0);
    glColor3f(color.redF(), color.greenF(), color.blueF());

    // draw arrows
    if(1 == axisNum)
    {
        width *= 0.5f;

        glRotated(-90.0, 1.0, 0.0, 0.0);

        if(xAxis)
            glRotated(90.0, 0.0, 1.0, 0.0);

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
    else // draw quad
    {
        height *= 0.33;
        if(isPicking)
            height *= 1.25f;

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, -1.0f);

        glBegin(GL_QUADS);
        {
            glVertex3f(   0.0, height, 0.0);
            glVertex3f(   0.0,    0.0, 0.0);
            glVertex3f(height,    0.0, 0.0);
            glVertex3f(height, height, 0.0);
        }
        glEnd();

        glDisable(GL_POLYGON_OFFSET_FILL);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}
