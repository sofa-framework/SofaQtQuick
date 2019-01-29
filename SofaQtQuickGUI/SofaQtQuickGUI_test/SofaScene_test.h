#include <QtTest/QTest>

#include <SofaQtQuickGUI/SofaQtQuickGUI.h>

class SofaScene_qtTests : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase() ;
    void loadAScene() ;
    void loadInvalidScene() ;
    void loadMissingScene() ;
    void testSceneQueries() ;
    void cleanupTestCase() ;

private:
    SofaQtQuickGUI* m_qmltypes {nullptr};
};
