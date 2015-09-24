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

protected:
    SofaApplication(QObject* parent = 0);

public:
    ~SofaApplication();
    static SofaApplication* Instance();

private:
    static SofaApplication* OurInstance;

};

}

}

#endif // SOFAAPPLICATION_H
