#include "Manipulator2D_Rotation.h"
#include "Viewer.h"

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

void Manipulator2D_Rotation::draw(const Viewer& viewer) const
{
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
    const float width = 8.0f;

    const int resolution = 64;

    glDisable(GL_CULL_FACE);

    glLineWidth(width);
    glEnable(GL_LINE_SMOOTH);

    // draw a ring
    glBegin(GL_LINE_STRIP);
    {
        //front
        glColor3f(0.25, 0.5, 1.0);

        for(int i = 0; i < resolution; ++i)
        {
            float angle = i / (resolution - 1.0f) * 2.0f * M_PI;
            float alpha = qCos(angle);
            float beta  = qSin(angle);

            glVertex3f(radius * alpha, radius * beta, 0.0);
        }
    }
    glEnd();

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
