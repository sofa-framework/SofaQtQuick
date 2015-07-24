#include <GL/glew.h>
#include "Selectable.h"

namespace sofa
{

namespace qtquick
{

Selectable::Selectable(QObject* parent) : QObject(parent)
{

}

Selectable::~Selectable()
{

}

void Selectable::setPosition(const QVector3D& position)
{
    myPosition = position;
}

}

}
