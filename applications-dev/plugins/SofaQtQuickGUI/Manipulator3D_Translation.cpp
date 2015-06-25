#include "Manipulator3D_Translation.h"
#include "Viewer.h"

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

void Manipulator3D_Translation::draw(const Viewer& viewer) const
{
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
    float height = 0.1f;
    {
        QVector4D p0 = camera->projection() * camera->view() * QVector4D(position(), 1.0);
        QVector4D p1 = camera->projection() * camera->view() * QVector4D(position() + camera->right(), 1.0);
        QVector3D direction = ((p1 / p1.w() - p0 / p0.w()).toVector3D());

        height *= 1.0 / direction.length();
    }

    const float width = 8.0f;

    glDisable(GL_CULL_FACE);

    glLineWidth(width);

    QColor color(xAxis ? 255 : 0, yAxis ? 255 : 0, zAxis ? 255 : 0);
    glColor3f(color.redF(), color.greenF(), color.blueF());

    // draw arrows
    if(1 == axisNum)
    {
        if(xAxis)
            glRotated(90.0, 0.0, 1.0, 0.0);
        else if(yAxis)
            glRotated(-90.0, 1.0, 0.0, 0.0);

        glBegin(GL_LINES);
        {
            glVertex3f( 0.0,            0.0,    0.0);
            glVertex3f( 0.0,            0.0,    height);

            glVertex3f(-0.2 * height,   0.0,    0.8 * height);
            glVertex3f( 0.0,            0.0,    height);

            glVertex3f( 0.2 * height,   0.0,    0.8 * height);
            glVertex3f( 0.0,            0.0,    height);
        }
        glEnd();
    }
    else // draw quad surrounded by lines
    {
        height *= 0.25;

        if(!xAxis)
            glRotated(-90.0, 0.0, 1.0, 0.0);
        else if(!yAxis)
            glRotated(90.0, 1.0, 0.0, 0.0);

        glBegin(GL_QUADS);
        {
            glVertex3f( height * 0.5, height * 0.5, 0.0);
            glVertex3f( height      , height * 0.5, 0.0);
            glVertex3f( height      , height      , 0.0);
            glVertex3f( height * 0.5, height      , 0.0);
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        {
            glVertex3f( height,    0.0, 0.0);
            glVertex3f( height, height, 0.0);
            glVertex3f(    0.0, height, 0.0);
        }
        glEnd();
    }

    glLineWidth(1.0f);

    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}
