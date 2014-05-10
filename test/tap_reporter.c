#include <stdlib.h> // malloc, free
#include <stddef.h> // NULL
#include <stdio.h> // stdout
#include <assert.h>

#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/tap_reporter.h>
#include <dummy/require.h>


void PassingTest()
{
}

void AssertTest()
{
    assert(0);
}

void FailTest()
{
    dummyAbortTest(DUMMY_FAIL_TEST, "Fail reason");
}

void PassingRequireTest()
{
    dummyRequire(2+2 == 4);
}

void FailingRequireTest()
{
    dummyRequire(2+2 == 5);
}

void SkipTest()
{
    dummyAbortTest(DUMMY_SKIP_TEST, "Skip reason");
}

void TodoWithoutReasonTest()
{
    dummyMarkTestAsTodo(NULL);
}

void TodoWithReasonTest()
{
    dummyMarkTestAsTodo("TODO reason");
}

int main()
{
    dummyInit(dummyGetTAPReporter(stdout));
    dummyAddTest("PassingTest", dummySignalSandbox, PassingTest);
    dummyAddTest("AssertTest", dummySignalSandbox, AssertTest);
    dummyAddTest("FailTest", dummySignalSandbox, FailTest);
    dummyAddTest("PassingRequireTest", dummySignalSandbox, PassingRequireTest);
    dummyAddTest("FailingRequireTest", dummySignalSandbox, FailingRequireTest);
    dummyAddTest("SkipTest", dummySignalSandbox, SkipTest);
    dummyAddTest("TodoWithoutReasonTest", dummySignalSandbox, TodoWithoutReasonTest);
    dummyAddTest("TodoWithReasonTest", dummySignalSandbox, TodoWithReasonTest);
    dummyRunTests();
    return 0;
}
