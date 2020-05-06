#include <QtTest/QTest>

class SofaScene_qtTests : public QObject
{
    Q_OBJECT
public:
    SofaScene_qtTests();

private slots:
    void loadAScene() ;
    void loadInvalidScene() ;
    void loadMissingScene() ;
    void testSceneQueries() ;
};
