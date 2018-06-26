#include "SofaScene_test.h"

class Toto : public BaseTest{
public:
    void qtTests(){
        SofaScene_test t;
        QTest::qExec(&t, {"-platform offscreen"});
    }
};

TEST_F(Toto, qtTests)
{
    Toto::qtTests();
}

