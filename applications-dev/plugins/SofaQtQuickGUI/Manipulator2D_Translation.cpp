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
    myXAxis(true),
    myYAxis(true)
{

}

Manipulator2D_Translation::~Manipulator2D_Translation()
{

}

void Manipulator2D_Translation::setXAxis(bool newXAxis)
{
    if(newXAxis == myXAxis)
        return;

    myXAxis = newXAxis;

    xAxisChanged(newXAxis);
}

void Manipulator2D_Translation::setYAxis(bool newYAxis)
{
    if(newYAxis == myYAxis)
        return;

    myYAxis = newYAxis;

    yAxisChanged(newYAxis);
}

void Manipulator2D_Translation::draw(const Viewer& viewer) const
{
    Camera* camera = viewer.camera();
    if(!camera)
        return;

    if(!myXAxis && !myYAxis)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    QVector4D position = camera->projection() * camera->view() * QVector4D(Manipulator::position(), 1.0);
    position = position / position.w();

    glLoadIdentity();
    glTranslatef(position.x(), position.y(), position.z());
    glScalef(viewer.height() / viewer.width(), 1.0f, 1.0f);

    // scale
    QVector3D scale = Manipulator::scale();
    glScaled(scale.x(), scale.y(), scale.z());

    // object
    float height = 0.1f;
    float width = 0.01f;

    if(myXAxis && !myYAxis)
        glScaled(height, width, width);
    else if(!myXAxis && myYAxis)
        glScaled(width, height, width);
    else
        glScaled(height * 0.5, height * 0.5, width);

    glDepthFunc(GL_ALWAYS);

    // draw arrows
    glBegin(GL_QUADS);
    {
        if(myXAxis && myYAxis)
            glColor3f(1.0, 1.0, 1.0);
        else if(myXAxis)
            glColor3f(1.0, 0.0, 0.0);
        else
            glColor3f(0.0, 1.0, 0.0);

        glVertex3f( 1.0, 1.0, 0.0);
        glVertex3f(-1.0, 1.0, 0.0);
        glVertex3f(-1.0,-1.0, 0.0);
        glVertex3f( 1.0,-1.0, 0.0);
    }
    glEnd();

    glDepthFunc(GL_LESS);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}
