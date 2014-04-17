#include <stddef.h> // NULL
#include <assert.h>
#include "../test/core.h"
#include "../test/require.h"
#include "../test/max_reporter.h"

void PassingTest()
{
}

void AssertTest()
{
    assert(0);
}

void FailTest()
{
    testAbortCase(TEST_FAIL_CASE, NULL);
}

void FailWithReasonTest()
{
    testAbortCase(TEST_FAIL_CASE, "Failed with a reason");
}

void PassingRequireTest()
{
    testRequire(2+2 == 4);
}

void FailingRequireTest()
{
    testRequire(2+2 == 5);
}

void SkipTest()
{
    testAbortCase(TEST_SKIP_CASE, NULL);
}

void SkipWithReasonTest()
{
    testAbortCase(TEST_SKIP_CASE, "Skipped with a reason");
}

void TodoTest()
{
    testMarkCaseAsTodo(NULL);
}

void TodoWithReasonTest()
{
    testMarkCaseAsTodo("TODO with a reason");
}

int main()
{
    testInit(testGetMaxReporter());
    testAddCase("PassingTest", PassingTest);
    testAddCase("AssertTest", AssertTest);
    testAddCase("FailTest", FailTest);
    testAddCase("FailWithReasonTest", FailWithReasonTest);
    testAddCase("PassingRequireTest", PassingRequireTest);
    testAddCase("FailingRequireTest", FailingRequireTest);
    testAddCase("SkipTest", SkipTest);
    testAddCase("SkipWithReasonTest", SkipWithReasonTest);
    testAddCase("TodoTest", TodoTest);
    testAddCase("TodoWithReasonTest", TodoWithReasonTest);
    testRun();
    return 0;
}
