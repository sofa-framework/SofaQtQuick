#include <sofa/helper/testing/BaseTest.h>
using sofa::helper::testing::BaseTest ;

#include <SofaQtQuickGUI/SofaScene.h>
using sofa::qtquick::SofaScene ;

#include <SofaQtQuickGUI/SofaSceneListModel.h>
using sofa::qtquick::SofaSceneListModel ;

#include <QApplication>

#include "modeltest.h"

class SofaSceneModel_test : public BaseTest
{
public:
    void checkGetComponents(){
        std::cout << SOFAQTQUICK_TEST_DIR << std::endl ;
        SofaScene scene;
        scene.setSource(QUrl(QString("file://")+QString(SOFAQTQUICK_TEST_DIR)+QString("/test1.xml")));

        SofaSceneListModel model;
        model.setSofaScene(&scene);

        ModelTest modeltest(&model) ;

        QMetaObject::invokeMethod( &modeltest, "runAllTests" );
    }
};

TEST_F(SofaSceneModel_test, checkGetComponents)
{
    checkGetComponents() ;
}
