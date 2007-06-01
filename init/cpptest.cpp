#include <cpptest.h>

CPPEmt7an myStaticObject[3];

CPPTest::CPPTest() 
{ 
    printf("in CPPTest constructor\n");
}

void CPPTest::Test()
{
    printf("in CPPTest test\n");
}

CPPEmt7an::CPPEmt7an()
{
    printf("in CPPEmt7an consturctor\n");
}

void CPPEmt7an::Test()
{
    printf("in CPPEmt7an test");
}

extern "C" void call_ctors();

extern "C"
void cppmain()
{    
    printf("now in cppmain\n");
    call_ctors();
    //CPPTest *test;
    //CPPEmt7an emt7an;
    //test = &emt7an;
    //test->Test();
    myStaticObject[0].Test();
}