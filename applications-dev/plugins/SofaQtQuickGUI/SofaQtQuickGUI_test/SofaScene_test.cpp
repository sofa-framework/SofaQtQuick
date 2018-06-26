#include "SofaScene_test.h"
#include <QtTest/QTest>

class SofaScene_test : public QObject, public BaseTest
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


TEST_F(SofaScene_test, qtTests)
{
    QTest::qExec(this) ;
}

