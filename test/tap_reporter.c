#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/tap_reporter.h>

#include "reporter_shared_tests.h"


int main()
{
    dummyInit(dummyGetTAPReporter(stdout));
    RegisterReporterTests();
    dummyRunTests();
    return 0;
}
