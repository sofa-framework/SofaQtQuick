#include "SofaApplication.h"

#include <QCoreApplication>
#include <QDir>

namespace sofa
{

namespace qtquick
{

SofaApplication* SofaApplication::OurInstance = nullptr;

SofaApplication::SofaApplication(QObject* parent) : QObject(parent)
{
    OurInstance = this;
}

SofaApplication::~SofaApplication()
{
    if(this == OurInstance)
        OurInstance = nullptr;
}

SofaApplication* SofaApplication::Instance()
{
    return OurInstance;
}

QString SofaApplication::binaryDirectory() const
{
    return QCoreApplication::applicationDirPath();
}

}

}
