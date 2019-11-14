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

#include "Manipulator3D_InPlaneTranslation.h"
#include <SofaQtQuickGUI/SofaViewer.h>

#include <sofa/core/visual/DrawToolGL.h>

#include <QApplication>
#include <GL/glew.h>
#include <QMatrix4x4>
#include <QDebug>

namespace sofaqtquick
{

Manipulator3D_InPlaneTranslation::Manipulator3D_InPlaneTranslation(QObject* parent)
    : Manipulator(parent)
{
}

Manipulator3D_InPlaneTranslation::~Manipulator3D_InPlaneTranslation()
{
}

void Manipulator3D_InPlaneTranslation::draw(const SofaViewer& viewer) const
{
    internalDraw(viewer, false);
}

void Manipulator3D_InPlaneTranslation::pick(const SofaViewer& viewer) const
{
    internalDraw(viewer, true);
}

void Manipulator3D_InPlaneTranslation::internalDraw(const SofaViewer& viewer, bool isPicking) const
{
    sofa::core::visual::DrawToolGL drawTools;

    if(!visible())
        return;

    Camera* camera = viewer.camera();
    if(!camera)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(position().x(), position().y(), position().z());

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

    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_COLOR_MATERIAL);

    QVector4D a, b, c, d, aa,bb,cc,dd;
    a = QVector4D(0.0,0.0,0.0,1.0);
    b = QVector4D(camera->up() * height, 1.0);
    c = QVector4D(camera->up() * height + camera->right() * (height/4.0f), 1.0);
    d = QVector4D(camera->right() * (height/4.0f), 1.0);

    aa = QVector4D(0.0,0.0,0.0,1.0);
    bb = QVector4D(camera->up() * (height / 4.0f), 1.0);
    cc = QVector4D(camera->up() * (height/4.0f) + camera->right() * height, 1.0);
    dd = QVector4D(camera->right() * height, 1.0);

    // draw arrows
    glBegin(GL_QUADS);
    {
        glColor3f(1.0f,0.0f,0.0f);
        glVertex3f(a.x(), a.y(), a.z());
        glVertex3f(b.x(), b.y(), b.z());
        glVertex3f(c.x(), c.y(), c.z());
        glVertex3f(d.x(), d.y(), d.z());

        glColor3f(0.0f,1.0f,0.0f);
        glVertex3f(aa.x(), aa.y(), aa.z());
        glVertex3f(bb.x(), bb.y(), bb.z());
        glVertex3f(cc.x(), cc.y(), cc.z());
        glVertex3f(dd.x(), dd.y(), dd.z());
    }
    glEnd();

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glLineWidth(1.0f);

    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}  // namespace sofaqtquick
