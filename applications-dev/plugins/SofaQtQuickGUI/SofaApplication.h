#ifndef SOFAAPPLICATION_H
#define SOFAAPPLICATION_H

#include "SofaQtQuickGUI.h"

namespace sofa
{

namespace qtquick
{

class SOFA_SOFAQTQUICKGUI_API SofaApplication : public QObject
{
    Q_OBJECT

public:
    static SofaApplication* Instance();

protected:
    SofaApplication(QObject* parent = 0);

public:
    ~SofaApplication();

private:
    static SofaApplication* OurInstance;

};

}

}

#endif // SOFAAPPLICATION_H
