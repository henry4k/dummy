#include <stddef.h> // NULL

#include <dummy/core.h>
#include <dummy/require.h>
#include <dummy/signal_sandbox.h>
#include <dummy/exception_sandbox.h>

#include "test_reporter.h"


dummySandbox level1Sandbox = NULL;
dummySandbox level2Sandbox = NULL;


void Level2()
{
    dummyLog("Level2 called!");
}

void Level1()
{
    dummyLog("Level1 called!");
    level2Sandbox(Level2, NULL);
    dummyRequire(2+2 == 5);
}

void signal_test()
{
    level1Sandbox = dummySignalSandbox;
    level2Sandbox = dummySignalSandbox;
    level1Sandbox(Level1, NULL);
}

void exception_test()
{
    level1Sandbox = dummyExceptionSandbox;
    level2Sandbox = dummyExceptionSandbox;
    level1Sandbox(Level1, NULL);
}

void signal_exception_test()
{
    level1Sandbox = dummySignalSandbox;
    level2Sandbox = dummyExceptionSandbox;
    level1Sandbox(Level1, NULL);
}

void exception_signal_test()
{
    level1Sandbox = dummyExceptionSandbox;
    level2Sandbox = dummySignalSandbox;
    level1Sandbox(Level1, NULL);
}

int main()
{
    dummyInit(dummyGetTestReporter());
    dummyAddTest("signal_test", dummySignalSandbox, signal_test);
    dummyAddTest("exception_test", dummySignalSandbox, exception_test);
    dummyAddTest("signal_exception_test", dummySignalSandbox, signal_exception_test);
    dummyAddTest("exception_signal_test", dummySignalSandbox, exception_signal_test);
    dummyRunTests();
    return 0;
}
