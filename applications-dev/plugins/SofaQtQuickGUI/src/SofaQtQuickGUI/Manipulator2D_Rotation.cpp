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

#include "Manipulator2D_Rotation.h"
#include "SofaViewer.h"

#include <QApplication>
#include <GL/glew.h>
#include <QMatrix4x4>
#include <qmath.h>
#include <QDebug>

namespace sofa
{

namespace qtquick
{

Manipulator2D_Rotation::Manipulator2D_Rotation(QObject* parent) : Manipulator(parent),
    myDisplayMark(false),
    myFromMarkAngle(0.0f),
    myToMarkAngle(0.0f)
{

}

Manipulator2D_Rotation::~Manipulator2D_Rotation()
{

}

void Manipulator2D_Rotation::setMark(float fromAngle, float toAngle)
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

void Manipulator2D_Rotation::unsetMark()
{
    myDisplayMark = false;

    myFromMarkAngle = 0.0f;
    myToMarkAngle = 0.0f;
}

void Manipulator2D_Rotation::draw(const SofaViewer& viewer) const
{
    internalDraw(viewer, false);
}

void Manipulator2D_Rotation::pick(const SofaViewer& viewer) const
{
    internalDraw(viewer, true);
}

void Manipulator2D_Rotation::internalDraw(const SofaViewer& viewer, bool isPicking) const
{
    if(!visible())
        return;

    Camera* camera = viewer.camera();
    if(!camera)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    QVector4D position = camera->projection() * camera->view() * QVector4D(Manipulator::position(), 1.0);
    position = position / position.w();

    glTranslatef(position.x(), position.y(), position.z());
    glScalef(viewer.height() / viewer.width(), 1.0f, 1.0f);

    // object
    const float radius = 0.2f;

    const float width = 4.0f * qApp->devicePixelRatio();

    const int resolution = 64;

    glDisable(GL_CULL_FACE);

    glLineWidth(width);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

    // draw a ring
    glBegin(GL_LINE_STRIP);
    {
        //front
        glColor4f(0.25f, 0.5f, 1.0f, 1.0f);

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

            glColor4f(0.25, 0.5, 1.0, 0.0);
            glVertex3f(innerRadius * alpha, innerRadius * beta, 0.0);

            glColor4f(0.25, 0.5, 1.0, 1.0);
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

            glColor4f(0.25, 0.5, 1.0, 0.0);
            glVertex3f(outerRadius * alpha, outerRadius * beta, 0.0);

            glColor4f(0.25, 0.5, 1.0, 1.0);
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
                float angle = myFromMarkAngle + i / (resolution - 1.0f) * 2.0f * M_PI;
                if(angle >= myToMarkAngle)
                {
                    angle = myToMarkAngle;
                    stop = true;
                }

                float alpha = qSin(angle);
                float beta  = qCos(angle);

                glVertex3f(radius * alpha, radius * beta, 0.0);

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
