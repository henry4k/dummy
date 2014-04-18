#include <stddef.h> // NULL
#include <assert.h>
#include "../lamer/core.h"
#include "../lamer/require.h"
#include "../lamer/max_reporter.h"

void PassingTest()
{
}

void AssertTest()
{
    assert(0);
}

void FailTest()
{
    lamerAbortTest(LAMER_FAIL_TEST, NULL);
}

void FailWithReasonTest()
{
    lamerAbortTest(LAMER_FAIL_TEST, "Failed with a reason");
}

void PassingRequireTest()
{
    lamerRequire(2+2 == 4);
}

void FailingRequireTest()
{
    lamerRequire(2+2 == 5);
}

void SkipTest()
{
    lamerAbortTest(LAMER_SKIP_TEST, NULL);
}

void SkipWithReasonTest()
{
    lamerAbortTest(LAMER_SKIP_TEST, "Skipped with a reason");
}

void TodoTest()
{
    lamerMarkTestAsTodo(NULL);
}

void TodoWithReasonTest()
{
    lamerMarkTestAsTodo("TODO with a reason");
}

int main()
{
    lamerInit(lamerGetMaxReporter());
    lamerAddTest("PassingTest", PassingTest);
    lamerAddTest("AssertTest", AssertTest);
    lamerAddTest("FailTest", FailTest);
    lamerAddTest("FailWithReasonTest", FailWithReasonTest);
    lamerAddTest("PassingRequireTest", PassingRequireTest);
    lamerAddTest("FailingRequireTest", FailingRequireTest);
    lamerAddTest("SkipTest", SkipTest);
    lamerAddTest("SkipWithReasonTest", SkipWithReasonTest);
    lamerAddTest("TodoTest", TodoTest);
    lamerAddTest("TodoWithReasonTest", TodoWithReasonTest);
    lamerRunTests();
    return 0;
}
