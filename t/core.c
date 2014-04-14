#include <stdio.h> // setvbuf
#include <stddef.h> // NULL
#include <assert.h>
#include "../test/core.h"
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
    testFailCase(NULL);
}

void FailWithReasonTest()
{
    testFailCase("Failed with a reason");
}

void SkipTest()
{
    testSkipCase(NULL);
}

void SkipWithReasonTest()
{
    testSkipCase("Skipped with a reason");
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
    // Disable buffering, so stdout and stderr are merged correctly
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    testInit(testGetMaxReporter());
    testAddCase("PassingTest", PassingTest);
    testAddCase("AssertTest", AssertTest);
    testAddCase("FailTest", FailTest);
    testAddCase("FailWithReasonTest", FailWithReasonTest);
    testAddCase("SkipTest", SkipTest);
    testAddCase("SkipWithReasonTest", SkipWithReasonTest);
    testAddCase("TodoTest", TodoTest);
    testAddCase("TodoWithReasonTest", TodoWithReasonTest);
    testRun();
    return 0;
}
