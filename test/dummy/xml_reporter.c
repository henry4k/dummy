#include <dummy/core.h>
#include <dummy/signal_sandbox.h>
#include <dummy/xml_reporter.h>

#include "reporter_shared_tests.h"


int main()
{
    dummyInit(dummyGetXMLReporter(stdout, "    ", "\n"));
    RegisterReporterTests();
    dummyRunTests();
    return 0;
}
