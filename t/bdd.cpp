#include "../lamer/core.h"
#include "../lamer/max_reporter.h"
#include "../lamer/bdd.hpp"

using namespace lamer;

int main()
{
    lamerInit(lamerGetMaxReporter());

    Describe("fizzbuzzer module")
        .it("does things", [](){
            // try doing things here
        })
        .it("is a slacker", [](){
            lamerMarkTestAsTodo("Implement lazyness");
            // prove that fizzbuzzer is lazy as fuck
        })
        .it("can't handle bloops", [](){
            lamerMarkTestAsTodo("Implement exception");
            // lamer if bloops cause fizzbuzzer to fail
            lamerAbortTest(LAMER_SKIP_TEST, "Exception not implemented yet");
        });

    Describe("bloop")
        .it("can make a sound", [](){
            // emulate a sound
            lamerAbortTest(LAMER_FAIL_TEST, "Bloop is silent!");
        })
        .it("looks weird", [](){
            lamerMarkTestAsTodo("Bloop needs to look weirder.");
            // bloops should always look weird
            lamerAbortTest(LAMER_FAIL_TEST, "Bloop doesn't looks weird enough!");
        });

    lamerRunTests();
}
