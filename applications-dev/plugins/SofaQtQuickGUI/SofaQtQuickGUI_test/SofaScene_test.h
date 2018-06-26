#include <sofa/helper/testing/BaseTest.h>
using sofa::helper::testing::BaseTest ;


#include <SofaQtQuickGUI/SofaScene.h>
using sofa::qtquick::SofaScene ;

#include <QtTest/QTest>

class SofaScene_test : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    { qDebug("called before everything else"); }
    void myFirstTest()
    {
        SofaScene scene;
        scene.setSource(QUrl(QString("file://")+QString(SOFAQTQUICK_TEST_DIR)+QString("/test1.xml")));
        //scene.animate();

    }
    void mySecondTest()
    { QVERIFY(1 != 2); }
    void cleanupTestCase()
    { qDebug("called after myFirstTest and mySecondTest"); }
};
