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

#include "Manipulator3D_Rotation.h"
#include "SofaViewer.h"

#include <QApplication>
#include <GL/glew.h>
#include <QMatrix4x4>
#include <QScreen>
#include <qmath.h>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

Manipulator3D_Rotation::Manipulator3D_Rotation(QObject* parent) : Manipulator(parent),
    myAxis("x"),
    myDisplayMark(false),
    myFromMarkAngle(0.0f),
    myToMarkAngle(0.0f)
{

}

Manipulator3D_Rotation::~Manipulator3D_Rotation()
{

}

void Manipulator3D_Rotation::setAxis(QString newAxis)
{
    if(newAxis == myAxis)
        return;

    myAxis = newAxis;

    axisChanged(newAxis);
}


void Manipulator3D_Rotation::setMark(float fromAngle, float toAngle)
{
    myFromMarkAngle = fromAngle;
    myToMarkAngle = toAngle;

    if(myFromMarkAngle > myToMarkAngle)
    {
        myFromMarkAngle = fmod(myFromMarkAngle, 2.0 * M_PI);
        myToMarkAngle   = myToMarkAngle + 2.0 * M_PI;
    }

    myDisplayMark = true;
}

void Manipulator3D_Rotation::unsetMark()
{
    myDisplayMark = false;

    myFromMarkAngle = 0.0f;
    myToMarkAngle = 0.0f;
}

void Manipulator3D_Rotation::draw(const SofaViewer& viewer) const
{
    internalDraw(viewer, false);
}

void Manipulator3D_Rotation::pick(const SofaViewer& viewer) const
{
    internalDraw(viewer, true);
}

void Manipulator3D_Rotation::internalDraw(const SofaViewer& viewer, bool isPicking) const
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

    if(1 != axisNum)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(position().x(), position().y(), position().z());

    QVector3D axis = QVector3D(0.0, 0.0, -1.0);
    if(xAxis)
        axis = QVector3D(-1.0, 0.0, 0.0);
    else if(yAxis)
        axis = QVector3D(0.0, -1.0, 0.0);

    bool invertedZ = false;
    if(QVector3D::dotProduct(camera->direction(), axis) < 0.0)
        invertedZ = true;

    // object
    if(xAxis)
        glRotated(90.0, 0.0, 1.0, 0.0);
    else if(yAxis)
        glRotated(-90.0, 1.0, 0.0, 0.0);

    const float width = 4.0f * qApp->devicePixelRatio();

    float radius = 0.125f;
    {
        QVector4D p0 = camera->projection() * camera->view() * QVector4D(position(), 1.0);
        QVector4D p1 = camera->projection() * camera->view() * QVector4D(position() + camera->up(), 1.0);
        QVector3D direction = ((p1 / p1.w() - p0 / p0.w()).toVector3D());

        radius *= 1.0 / direction.length();
    }

    const int resolution = 64;

    glDisable(GL_CULL_FACE);

    glLineWidth(width);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);

    QColor color(xAxis ? 255 : 50, yAxis ? 255 : 50, zAxis ? 255 : 50);

    // draw a ring
    glBegin(GL_LINE_STRIP);
    {
        glColor3f(color.redF(), color.greenF(), color.blueF());

        for(int i = 0; i < resolution; ++i)
        {
            float angle = i / (resolution - 1.0f) * 2.0f * M_PI;
            float alpha = qCos(angle);
            float beta  = qSin(angle);

            glVertex3f(radius * alpha, radius * beta, 0.0);
        }
    }
    glEnd();

    if(!isPicking)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    float thickness = 0.1f;
    if(isPicking)
        thickness *= 1.5f;

    float innerRadius = (1.0f - thickness) * radius;
    glBegin(GL_QUAD_STRIP);
    {
        //front
        for(int i = 0; i < resolution; ++i)
        {
            float angle = i / (resolution - 1.0f) * 2.0f * M_PI;
            float alpha = qCos(angle);
            float beta  = qSin(angle);

            glColor4f(color.redF(), color.greenF(), color.blueF(), 0.0f);
            glVertex3f(innerRadius * alpha, innerRadius * beta, 0.0);

            glColor4f(color.redF(), color.greenF(), color.blueF(), 1.0f);
            glVertex3f(radius * alpha, radius * beta, 0.0);
        }
    }
    glEnd();

    float outerRadius = (1.0f + thickness) * radius;
    glBegin(GL_QUAD_STRIP);
    {
        //front
        for(int i = 0; i < resolution; ++i)
        {
            float angle = i / (resolution - 1.0f) * 2.0f * M_PI;
            float alpha = qCos(angle);
            float beta  = qSin(angle);

            glColor4f(color.redF(), color.greenF(), color.blueF(), 0.0f);
            glVertex3f(outerRadius * alpha, outerRadius * beta, 0.0);

            glColor4f(color.redF(), color.greenF(), color.blueF(), 1.0f);
            glVertex3f(radius * alpha, radius * beta, 0.0);
        }
    }
    glEnd();

    if(!isPicking)
        glDisable(GL_BLEND);

    // draw a ring portion to know the delta angle
    if(myDisplayMark)
    {
        glDepthRange(0.0, 0.9999995);
        glDepthFunc(GL_ALWAYS);

        // draw a ring
        glBegin(GL_LINE_STRIP);
        {
            //front
            glColor3f(1.0, 1.0, 1.0);

            bool stop = false;
            for(int i = 0; i < resolution; ++i)
            {
                float angle = myFromMarkAngle;

                if(!invertedZ)
                {
                    angle += i / (resolution - 1.0f) * 2.0f * M_PI;

                    if(angle >= myToMarkAngle)
                    {
                        angle = myToMarkAngle;
                        stop = true;
                    }
                }
                else
                {
                    angle -= i / (resolution - 1.0f) * 2.0f * M_PI;

                    if(angle <= myToMarkAngle - 2 * M_PI)
                    {
                        angle = myToMarkAngle - 2 * M_PI;
                        stop = true;
                    }
                }

                float alpha = qSin(angle);
                float beta  = qCos(angle);

                glVertex3f(radius * alpha, radius * beta, 0.0f);

                if(stop)
                    break;
            }
        }
        glEnd();

        glDepthFunc(GL_LESS);
        glDepthRange(0.0, 1.0);
    }

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LINE_SMOOTH);
    glLineWidth(1.0f);

    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}
