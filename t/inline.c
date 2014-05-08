#include <dummy/core.h>
#include <dummy/exception_sandbox.h>
#include <dummy/inline.h>

#include "test_reporter.h"


DUMMY_INLINE_TEST("Fizzbuzzer does things")
{
    // try doing things here
}

DUMMY_INLINE_TEST("Fizzbuzzer is a slacker")
{
    dummyMarkTestAsTodo("Implement lazyness");
    // prove that fizzbuzzer is lazy as fuck
}

DUMMY_INLINE_TEST("Fizzbuzzer can't handle bloops")
{
    dummyMarkTestAsTodo("Implement exception");
    // dummy if bloops cause fizzbuzzer to fail
    dummyAbortTest(DUMMY_SKIP_TEST, "Exception not implemented yet");
}

int main()
{
    dummyInit(dummyGetExceptionSandbox(), dummyGetTestReporter());
    dummyAddInlineTests();
    dummyRunTests();
    return 0;
}
