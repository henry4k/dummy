#include <stdlib.h> // malloc, free
#include <stddef.h> // NULL
#include <assert.h>

#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/require.h>

#include "test_reporter.h"


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

int* CreateInt( int value )
{
    int* value_ = (int*)malloc(sizeof(int));
    *value_ = value;
    dummyLog("created %d", value);
    return value_;
}

void CleanupInt( void* value_ )
{
    int* value = (int*)value_;
    dummyLog("cleaned %d", *value);
    free(value);
}

void PassingCleanupTest()
{
    void* data = CreateInt(42);
    dummyAddCleanup(CleanupInt, data);
}

void FailingCleanupTest()
{
    void* data = CreateInt(42);
    dummyAddCleanup(CleanupInt, data);
    dummyAbortTest(DUMMY_FAIL_TEST, "Fail reason");
}

int main()
{
    dummyInit(dummyGetTestReporter());
    dummyAddTest("PassingTest", dummySignalSandbox, PassingTest);
    dummyAddTest("AssertTest", dummySignalSandbox, AssertTest);
    dummyAddTest("FailTest", dummySignalSandbox, FailTest);
    dummyAddTest("PassingRequireTest", dummySignalSandbox, PassingRequireTest);
    dummyAddTest("FailingRequireTest", dummySignalSandbox, FailingRequireTest);
    dummyAddTest("SkipTest", dummySignalSandbox, SkipTest);
    dummyAddTest("TodoWithoutReasonTest", dummySignalSandbox, TodoWithoutReasonTest);
    dummyAddTest("TodoWithReasonTest", dummySignalSandbox, TodoWithReasonTest);
    dummyAddTest("PassingCleanupTest", dummySignalSandbox, PassingCleanupTest);
    dummyAddTest("FailingCleanupTest", dummySignalSandbox, FailingCleanupTest);
    dummyRunTests();
    return 0;
}
