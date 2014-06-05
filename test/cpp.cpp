#include <stdexcept> // logic error

#include <dummy/core.h>
#include <dummy/exception_sandbox.hpp>

#include "test_reporter.h"


void UnknownExceptionTest()
{
    throw 42;
}

void StdExceptionTest()
{
    throw std::logic_error("This should fail");
}

void AbortTest()
{
    dummyAbortTest(DUMMY_FAIL_TEST, "This is the reason.");
}

class Foo
{
public:
    int m_Value;

    Foo( int value ) :
        m_Value(value)
    {
        dummyLog("created Foo with %d", value);
    }

    ~Foo()
    {
        dummyLog("destroyed Foo with %d", m_Value);
    }
};

void PassingDestructorTest()
{
    Foo foo(42);
}

void FailingDestructorTest()
{
    Foo foo(42);
    dummyAbortTest(DUMMY_FAIL_TEST, "Destructor test failed.");
}

int main()
{
    dummyInit(dummyGetTestReporter());
    dummyAddTest("UnknownExceptionTest", dummyExceptionSandbox, UnknownExceptionTest);
    dummyAddTest("StdExceptionTest", dummyExceptionSandbox, StdExceptionTest);
    dummyAddTest("AbortTest", dummyExceptionSandbox, AbortTest);
    dummyAddTest("PassingDestructorTest", dummyExceptionSandbox, PassingDestructorTest);
    dummyAddTest("FailingDestructorTest", dummyExceptionSandbox, FailingDestructorTest);
    dummyRunTests();
    return 0;
}
