#include <sofa/helper/testing/BaseTest.h>
using sofa::helper::testing::BaseTest ;

#include <SofaQtQuickGUI/SofaScene.h>
using sofa::qtquick::SofaScene ;

#include <SofaQtQuickGUI/QmlPluginSofa.h>
using sofaqtquick::QmlPluginSofa;

#include <sofa/helper/testing/BaseTest.h>
#include "SofaScene_test.h"

#include <chrono>
#include <thread>

SofaScene_qtTests::SofaScene_qtTests()
{
    QmlPluginSofa::Init();
}

void SofaScene_qtTests::loadAScene()
{
    SofaScene scene;

    QVERIFY(!scene.isReady());
    scene.setSource(QUrl(QString("file://")+QString(SOFAQTQUICK_TEST_DIR)+QString("/test_minimal.xml")));

    int cpt = 0;
    while(!scene.isReady() && cpt < 50){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); cpt++; };
    QVERIFY2(cpt < 50, "Loading of the scene timed out after 5 seconds.");
}

void SofaScene_qtTests::testSceneQueries()
{
    SofaScene scene;

    QVERIFY(!scene.isReady());
    scene.setSource(QUrl(QString("file://")+QString(SOFAQTQUICK_TEST_DIR)+QString("/test_minimal.xml")));

    int cpt = 0;
    while(!scene.isReady() && cpt < 50){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); cpt++; };
    QVERIFY2(cpt < 50, "Loading of the scene timed out after 5 seconds.");

    std::cout << scene.dumpGraph().toStdString() << std::endl;

    QVERIFY( scene.component("@/") != nullptr ) ;
    QVERIFY( scene.component("@/child1") != nullptr ) ;
    QVERIFY( scene.component("@/child2") != nullptr ) ;
    QVERIFY( scene.component("@/child1/child11") != nullptr ) ;
    QVERIFY( scene.component("@/child2/child11") == nullptr ) ;
}

void SofaScene_qtTests::loadInvalidScene()
{
    SofaScene scene;

    QVERIFY(!scene.isReady());
    scene.setSource(QUrl(QString("file://")+QString(SOFAQTQUICK_TEST_DIR)+QString("/test_broken.xml")));

    int cpt = 0;
    while(!scene.isOnError() && cpt < 50){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); cpt++; };

    QVERIFY2(cpt < 50, "Loading of the scene timed out after 5 seconds.");
    QVERIFY(!scene.isReady());
}

void SofaScene_qtTests::loadMissingScene()
{
    SofaScene scene;

    QVERIFY(!scene.isReady());
    scene.setSource(QUrl(QString("file://")+QString(SOFAQTQUICK_TEST_DIR)+QString("/not_existing.xml")));
    int cpt = 0;
    while(!scene.isOnError() && cpt < 50){ std::this_thread::sleep_for(std::chrono::milliseconds(100)); cpt++; };
    QVERIFY2(cpt < 50, "Loading of the scene timed out after 5 seconds.");

    QVERIFY(!scene.isReady());
}

TEST(SofaScene_tests, qtTest)
{
    SofaScene_qtTests tests;
    QTest::qExec(&tests,{"-platform offscreen"}) ;
}

