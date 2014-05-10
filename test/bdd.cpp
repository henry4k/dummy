#include <dummy/core.h>
#include <dummy/exception_sandbox.h>
#include <dummy/bdd.hpp>

#include "test_reporter.h"


using namespace dummy;

int main()
{
    dummyInit(dummyGetTestReporter());

    Describe("fizzbuzzer module")
        .use(dummyExceptionSandbox)

        .it("does things", [](){
            // try doing things here
        })
        .it("is a slacker", [](){
            dummyMarkTestAsTodo("Implement lazyness");
            // prove that fizzbuzzer is lazy as fuck
        })
        .it("can't handle bloops", [](){
            dummyMarkTestAsTodo("Implement exception");
            // test if bloops cause fizzbuzzer to fail
            dummyAbortTest(DUMMY_SKIP_TEST, "Exception not implemented yet");
        });

    Describe("bloop")
        .use(dummyExceptionSandbox)

        .it("can make a sound", [](){
            // emulate a sound
            dummyAbortTest(DUMMY_FAIL_TEST, "Bloop is silent!");
        })
        .it("looks weird", [](){
            dummyMarkTestAsTodo("Bloop needs to look weirder.");
            // bloops should always look weird
            dummyAbortTest(DUMMY_FAIL_TEST, "Bloop doesn't looks weird enough!");
        });

    dummyRunTests();
}
