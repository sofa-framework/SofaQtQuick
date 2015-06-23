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

    if(0 == axisNum)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    QVector3D position = Manipulator::position();

    glTranslatef(position.x(), position.y(), position.z());

    QVector3D axis = QVector3D(0.0, 0.0, 1.0);
    if(xAxis)
        axis = QVector3D(1.0, 0.0, 0.0);
    else if(yAxis)
        axis = QVector3D(0.0, 1.0, 0.0);

    // object
    if(xAxis)
        glRotated(90.0, 0.0, 1.0, 0.0);
    else if(yAxis)
        glRotated(-90.0, 1.0, 0.0, 0.0);

    float height = viewer.height() * 0.01f;
//    {
//        QVector4D p0 = viewer.camera()->projection() * viewer.camera()->view() * QVector4D(QVector3D(0.0, 0.0, 0.0), 1.0);
//        p0 /= p0.w();

//        QVector4D p1 = viewer.camera()->projection() * viewer.camera()->view() * QVector4D(axis, 1.0);
//        p1 /= p1.w();

//        height /= (p1 - p0).toVector3D().length();
//    }

    const float width = 8.0f;

    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_ALWAYS);

    glLineWidth(width);

    if(3 == axisNum)
        glColor3f(1.0, 1.0, 1.0);
    else if(2 == axisNum)
        glColor3f(0.7, 0.7, 0.7);
    else if(xAxis)
        glColor3f(1.0, 0.0, 0.0);
    else if(yAxis)
        glColor3f(0.0, 1.0, 0.0);
    else if(zAxis)
        glColor3f(0.0, 0.0, 1.0);

    // draw arrows
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

    glLineWidth(1.0f);

    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

}

}
