#include <stddef.h> // NULL
#include <dummy/core.h> // dummyAbortTest
#include "simple_stub.h"

void (*Bar_stub)(int) = NULL;
void Bar(int foo)
{
    if(!Bar_stub)
        dummyAbortTest(DUMMY_FAIL_TEST, "Called Bar without stub callback.");
    return Bar_stub(foo);
}

const char* (*Frobnicate_stub)(const char*) = NULL;
const char* Frobnicate(const char* str)
{
    if(!Frobnicate_stub)
        dummyAbortTest(DUMMY_FAIL_TEST, "Called Frobnicate without stub callback.");
    return Frobnicate_stub(str);
}

void (*MassFrobnicator_stub)(const char*, const int*) = NULL;
void MassFrobnicator(const char* aaa, const int* bbb)
{
    if(!MassFrobnicator_stub)
        dummyAbortTest(DUMMY_FAIL_TEST, "Called MassFrobnicator without stub callback.");
    return MassFrobnicator_stub(aaa, bbb);
}

