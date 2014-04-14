#include "../test/core.h"
#include "../test/max_reporter.h"
#include "../test/bdd.hpp"

using namespace test;

int main()
{
    testInit(testGetMaxReporter());

    Describe("fizzbuzzer module")
        .it("does things", [](){
            // try doing things here
        })
        .it("is a slacker", [](){
            testMarkCaseAsTodo("Implement lazyness");
            // prove that fizzbuzzer is lazy as fuck
        })
        .it("can't handle bloops", [](){
            testMarkCaseAsTodo("Implement exception");
            // test if bloops cause fizzbuzzer to fail
            testSkipCase("Exception not implemented yet");
        });

    Describe("bloop")
        .it("can make a sound", [](){
            // emulate a sound
            testFailCase("Bloop is silent!");
        })
        .it("looks weird", [](){
            testMarkCaseAsTodo("Bloop needs to look weirder.");
            // bloops should always look weird
            testFailCase("Bloop doesn't looks weird enough!");
        });

    testRun();
}
