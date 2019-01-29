#include <QtTest/QTest>

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>

class SofaSceneModel_qtTests : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase() ;
    void testAll() ;
    void cleanupTestCase() ;

private:
    SofaQtQuickGUI* m_qmltypes {nullptr};
};
