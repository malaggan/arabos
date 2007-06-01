#include <cpptest.h>

CPPTest::CPPTest() 
{ 
    printf("in CPPTest constructor\n");
}

void CPPTest::Test()
{
    printf("in CPPTest\n");
}

CPPEmt7an::CPPEmt7an()
{
    printf("in CPPEmt7an consturctor\n");
}

void CPPEmt7an::Test()
{
    printf("in CPPEmt7an");
}

extern "C"
void cppmain()
{
    CPPTest *test;
    CPPEmt7an emt7an;
    test = &emt7an;
    test->Test();
}