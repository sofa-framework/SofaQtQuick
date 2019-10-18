#include <sofa/helper/testing/BaseTest.h>
using sofa::helper::testing::BaseTest ;

#include <SofaQtQuickGUI/SofaScene.h>
using sofa::qtquick::SofaScene ;

#include <SofaQtQuickGUI/Models/SofaSceneListModel.h>
using sofa::qtquick::SofaSceneListModel ;

#include <QApplication>

#include <SofaQtQuickGUI/SofaQtQuickQmlModule.h>
using sofaqtquick::SofaQtQuickQmlModule;

#include "modeltest.h"
#include "SofaSceneModel_test.h"

#include <QQmlEngine>

#include <chrono>
#include <thread>

SofaSceneModel_qtTests::SofaSceneModel_qtTests()
{
    QQmlEngine engine;
    SofaQtQuickQmlModule::RegisterTypes(&engine);
}

void  SofaSceneModel_qtTests::testAll(){
    std::cout << SOFAQTQUICK_TEST_DIR << std::endl ;
    SofaScene scene;
    scene.setSource(QUrl(QString("file://")+QString(SOFAQTQUICK_TEST_DIR)+QString("/test_minimal.xml")));
    int cpt=0;
    while(!scene.isReady() && cpt < 50){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); cpt++; };
    EXPECT_LT(cpt,50);
    EXPECT_TRUE(scene.isReady());
    SofaSceneListModel model;
    model.setSofaScene(&scene);

    ModelTest modeltest(&model) ;

    //QMetaObject::invokeMethod( &modeltest, "runAllTests" );
}

TEST(SofaSceneModel_test, checkGetComponents)
{
    SofaSceneModel_qtTests tests;
    QTest::qExec(&tests,{"-platform offscreen"}) ;
}
