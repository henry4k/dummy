#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/require.h>

#include "test_reporter.h"


void SucceedTest()
{
    dummyRequire(1+1 == 2);
    dummyRequireIntEquals(2, 1+1);
    dummyRequireFloatEquals(2, 0.01, 1+1.0099);
    dummyRequireFloatEquals(2, 0.01, 2-0.0099);
    dummyRequireStringEquals("foo bar", "foo bar");
    dummyRequirePointerEquals(NULL, NULL);
}

void RequireTest()
{
    dummyRequire(1+1 == 3);
}

void RequireIntEqualsTest()
{
    dummyRequireIntEquals(3, 1+1);
}

void RequireFloatEqualsTest()
{
    dummyRequireFloatEquals(3, 0.01, 1+1.009);
}

void RequireStringEqualsTest()
{
    dummyRequireStringEquals("foo bar", "foo" "bar");
}

void RequirePointerEqualsTest()
{
    const int* ap = (const int*)1;
    const int* bp = NULL;
    dummyRequirePointerEquals(ap, bp);
}

int main()
{
    dummyInit(dummyGetTestReporter());
    dummyAddTest("SucceedTest", dummySignalSandbox, SucceedTest);
    dummyAddTest("RequireTest", dummySignalSandbox, RequireTest);
    dummyAddTest("RequireIntEqualsTest", dummySignalSandbox, RequireIntEqualsTest);
    dummyAddTest("RequireFloatEqualsTest", dummySignalSandbox, RequireFloatEqualsTest);
    dummyAddTest("RequireStringEqualsTest", dummySignalSandbox, RequireStringEqualsTest);
    dummyAddTest("RequirePointerEqualsTest", dummySignalSandbox, RequirePointerEqualsTest);
    dummyRunTests();
    return 0;
}
