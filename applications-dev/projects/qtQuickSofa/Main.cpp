#include <SofaQtQuickGUI/Tools.h>

using namespace sofa::qtquick;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QQmlApplicationEngine applicationEngine;

    // application specific settings
    app.setOrganizationName("Sofa");
    app.setApplicationName("qtQuickSofa");

    // common settings for most sofaqtquick applications
    if( !sofa::qtquick::Tools::basicMain( app, applicationEngine, "qrc:/qml/Main.qml" ) ) return -1;

    return app.exec();
}
