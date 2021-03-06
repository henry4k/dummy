#include <stdlib.h> // malloc, free
#include <stddef.h> // NULL
#include <stdio.h> // stdout
#include <assert.h>

#include <dummy/require.h>


void PassingTest()
{
}

void LogTest()
{
    dummyLog("aaa\nbbb\nccc");
}

void AssertTest()
{
    assert(0);
}

void FailTest()
{
    dummyAbortTest(DUMMY_FAIL_TEST, "Fail reason\nmultiple\nlines");
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
    dummyAbortTest(DUMMY_SKIP_TEST, "Skip reason\nmultiple\nlines");
}

void TodoWithoutReasonTest()
{
    dummyMarkTestAsTodo(NULL);
}

void TodoWithReasonTest()
{
    dummyMarkTestAsTodo("TODO reason\nmultiple\nlines");
}

void RegisterReporterTests()
{
    dummyAddTest("PassingTest", dummySignalSandbox, PassingTest);
    dummyAddTest("LogTest", dummySignalSandbox, LogTest);
    dummyAddTest("AssertTest", dummySignalSandbox, AssertTest);
    dummyAddTest("FailTest", dummySignalSandbox, FailTest);
    dummyAddTest("PassingRequireTest", dummySignalSandbox, PassingRequireTest);
    dummyAddTest("FailingRequireTest", dummySignalSandbox, FailingRequireTest);
    dummyAddTest("SkipTest", dummySignalSandbox, SkipTest);
    dummyAddTest("TodoWithoutReasonTest", dummySignalSandbox, TodoWithoutReasonTest);
    dummyAddTest("TodoWithReasonTest", dummySignalSandbox, TodoWithReasonTest);
}
