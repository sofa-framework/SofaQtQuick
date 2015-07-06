#include "Manipulator2D_Translation.h"
#include "Viewer.h"

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

void Manipulator2D_Translation::draw(const Viewer& viewer) const
{
    internalDraw(viewer, false);
}

void Manipulator2D_Translation::pick(const Viewer& viewer) const
{
    internalDraw(viewer, true);
}

void Manipulator2D_Translation::internalDraw(const Viewer& viewer, bool isPicking) const
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
    float width = 0.01f;

    if(-1 != myAxis.indexOf('x') && -1 != myAxis.indexOf('y'))
        glScaled(height * 0.5, height * 0.5, width);
    else if(-1 != myAxis.indexOf('x'))
        glScaled(height, width, width);
    else if(-1 != myAxis.indexOf('y'))
        glScaled(width, height, width);

    // draw arrows
    glBegin(GL_QUADS);
    {
        if(xAxis && yAxis)
            glColor3f(1.0, 1.0, 1.0);
        else if(xAxis)
            glColor3f(1.0, 0.0, 0.0);
        else if(yAxis)
            glColor3f(0.0, 1.0, 0.0);

        glVertex3f( 1.0, 1.0, 0.0);
        glVertex3f(-1.0, 1.0, 0.0);
        glVertex3f(-1.0,-1.0, 0.0);
        glVertex3f( 1.0,-1.0, 0.0);
    }
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}
