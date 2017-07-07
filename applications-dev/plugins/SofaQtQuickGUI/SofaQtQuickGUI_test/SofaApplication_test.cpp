#include <SofaTest/Sofa_test.h>
using sofa::Sofa_test ;

class SofaApplication_test : public Sofa_test<>
{
public:
    void checkGetComponents(){

    }
};

TEST_F(SofaApplication_test, checkGetComponents)
{
    checkGetComponents() ;
}

