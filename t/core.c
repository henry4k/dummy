#include <stddef.h> // NULL
#include <assert.h>
#include "../dummy/core.h"
#include "../dummy/require.h"
#include "../dummy/max_reporter.h"

void PassingTest()
{
}

void AssertTest()
{
    assert(0);
}

void FailTest()
{
    dummyAbortTest(DUMMY_FAIL_TEST, NULL);
}

void FailWithReasonTest()
{
    dummyAbortTest(DUMMY_FAIL_TEST, "Failed with a reason");
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
    dummyAbortTest(DUMMY_SKIP_TEST, NULL);
}

void SkipWithReasonTest()
{
    dummyAbortTest(DUMMY_SKIP_TEST, "Skipped with a reason");
}

void TodoTest()
{
    dummyMarkTestAsTodo(NULL);
}

void TodoWithReasonTest()
{
    dummyMarkTestAsTodo("TODO with a reason");
}

int main()
{
    dummyInit(dummyGetMaxReporter());
    dummyAddTest("PassingTest", PassingTest);
    dummyAddTest("AssertTest", AssertTest);
    dummyAddTest("FailTest", FailTest);
    dummyAddTest("FailWithReasonTest", FailWithReasonTest);
    dummyAddTest("PassingRequireTest", PassingRequireTest);
    dummyAddTest("FailingRequireTest", FailingRequireTest);
    dummyAddTest("SkipTest", SkipTest);
    dummyAddTest("SkipWithReasonTest", SkipWithReasonTest);
    dummyAddTest("TodoTest", TodoTest);
    dummyAddTest("TodoWithReasonTest", TodoWithReasonTest);
    dummyRunTests();
    return 0;
}
