/* 
 * File:   cpptest.h
 * Author: meemo
 *
 * Created on June 1, 2007, 7:52 PM
 */

#ifndef _cpptest_H
#define	_cpptest_H

#include <printf.h>

#ifndef __cplusplus
    void cppmain();
#endif /* !__cplusplus */

#ifdef __cplusplus

class CPPTest
{
public:
    CPPTest();
    virtual void Test();
};

class CPPEmt7an : public CPPTest
{
public:
    CPPEmt7an();
    void Test();
};

#endif /* __cplusplus */

#endif	/* _cpptest_H */

