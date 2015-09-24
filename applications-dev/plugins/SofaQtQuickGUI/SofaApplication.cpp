#include "SofaApplication.h"

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

}

}
