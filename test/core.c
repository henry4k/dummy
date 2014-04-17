#include <stdlib.h> // malloc, free
#include <string.h> // memset, strncpy
#include <stdio.h>  // testFormat etc
#include <stdarg.h> // testFormat etc
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <assert.h>
#include "core.h"


enum
{
    TEST_MAX_MESSAGE_LENGTH = 127,
    TEST_MAX_CLEANUPS = 32,
    TEST_MAX_CASES = 32,
    TEST_MAX_ENVIRONMENTS = 16,
    TEST_INVALID_CASE_INDEX = -1
};


typedef void (*testCleanupFunction)( void* data );

/**
 * Is called after the test case has been completed -
 * regardless of whether it failed or succeeded.
 */
typedef struct
{
    testCleanupFunction fn;
    void* data;
} testCleanup;

typedef struct
{
    char name[TEST_MAX_MESSAGE_LENGTH];
    testCaseFunction fn;
    testCaseStatus status;

    testCaseResult result;
    char abortReason[TEST_MAX_MESSAGE_LENGTH];

    bool markedAsTodo;
    char todoReason[TEST_MAX_MESSAGE_LENGTH];

    testCleanup cleanupStack[TEST_MAX_CLEANUPS];
    int cleanupStackSize;
} testCase;

typedef struct
{
    jmp_buf jumpBuffer;
} testEnvironment;

typedef struct
{
    const testReporter* reporter;
    testStatus status;

    testCase cases[TEST_MAX_CASES];
    int caseCount;

    int currentCaseIndex;

    testEnvironment environmentStack[TEST_MAX_ENVIRONMENTS];
    int environmentStackSize;
} testContext;


testContext* testCurrentContext = NULL;


bool testRunCase( int index );
bool testProtectedCall( testCaseFunction fn );
void testSignalHandler( int signal );

void testInit( const testReporter* reporter )
{
    assert(testCurrentContext == NULL);
    testCurrentContext = malloc(sizeof(testContext));
    memset(testCurrentContext, 0, sizeof(testContext));

    assert(reporter);
    assert(reporter->began);
    assert(reporter->completed);
    assert(reporter->beganCase);
    assert(reporter->completedCase);
    assert(reporter->diag);
    testCurrentContext->reporter = reporter;

    testCurrentContext->status = TEST_INITIALIZING;
    testCurrentContext->currentCaseIndex = TEST_INVALID_CASE_INDEX;
}

int testRun()
{
    testContext* ctx = testCurrentContext;
    assert(ctx);
    assert(ctx->status == TEST_INITIALIZING);

    ctx->reporter->began(ctx->reporter->context);

    ctx->status = TEST_RUNNING;
    int failedCases = 0;
    int i = 0;
    for(; i < ctx->caseCount; i++)
        if(testRunCase(i) == false)
            failedCases++;

    ctx->status = TEST_COMPLETED;
    ctx->reporter->completed(ctx->reporter->context);

    assert(ctx->status == TEST_COMPLETED);
    free(ctx);
    testCurrentContext = NULL;

    return failedCases;
}

int testAddCase( const char* name, testCaseFunction fn )
{
    testContext* ctx = testCurrentContext;
    assert(ctx);
    assert(ctx->status == TEST_INITIALIZING);
    assert(ctx->caseCount < TEST_MAX_CASES);

    testCase* tc = &ctx->cases[ctx->caseCount];
    memset(tc, 0, sizeof(testCase));

    strncpy(tc->name, name, TEST_MAX_MESSAGE_LENGTH);
    tc->fn = fn;

    ctx->caseCount++;

    return ctx->caseCount-1;
}

bool testRunCase( int index )
{
    testContext* ctx = testCurrentContext;
    assert(ctx);
    assert(ctx->status == TEST_RUNNING);

    assert(index >= 0);
    assert(index < TEST_MAX_CASES);
    testCase* tc = &ctx->cases[index];

    // prepare
    ctx->currentCaseIndex = index;
    tc->status = TEST_CASE_STARTING;
    tc->result = TEST_CASE_PASSED;
    ctx->reporter->beganCase(ctx->reporter->context);
    signal(SIGABRT, testSignalHandler);
    signal(SIGFPE, testSignalHandler);
    signal(SIGILL, testSignalHandler);
    signal(SIGSEGV, testSignalHandler);

    // run
    tc->status = TEST_CASE_RUNNING;
    const bool passed = testProtectedCall(tc->fn);
    if(!passed && tc->result == TEST_CASE_PASSED)
        tc->result = TEST_CASE_FAILED;
    tc->status = TEST_CASE_COMPLETED;

    // cleanup
    signal(SIGABRT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    ctx->reporter->completedCase(ctx->reporter->context);
    ctx->currentCaseIndex = TEST_INVALID_CASE_INDEX;
    tc->status = TEST_CASE_UNDEFINED;

    return passed;
}

testStatus testGetStatus()
{
    testContext* ctx = testCurrentContext;
    assert(ctx);

    return ctx->status;
}

int testGetCaseCount()
{
    const testContext* ctx = testCurrentContext;
    assert(ctx);

    return ctx->caseCount;
}

int testGetCaseNumber()
{
    const testContext* ctx = testCurrentContext;
    assert(ctx);

    assert(ctx->currentCaseIndex >= 0);
    assert(ctx->currentCaseIndex < TEST_MAX_CASES);

    return ctx->currentCaseIndex;
}

testCase* testGetCurrentCase()
{
    testContext* ctx = testCurrentContext;
    assert(ctx);

    assert(ctx->currentCaseIndex >= 0);
    assert(ctx->currentCaseIndex < TEST_MAX_CASES);
    testCase* tc = &ctx->cases[ctx->currentCaseIndex];

    return tc;
}

const char* testGetCaseName()
{
    const testCase* tc = testGetCurrentCase();
    assert(tc->name);
    assert(strlen(tc->name) < TEST_MAX_MESSAGE_LENGTH);
    return tc->name;
}

testCaseStatus testGetCaseStatus()
{
    return testGetCurrentCase()->status;
}

testCaseResult testGetCaseResult()
{
    const testCase* tc = testGetCurrentCase();
    assert(tc->status == TEST_CASE_COMPLETED);
    return tc->result;
}

const char* testGetCaseAbortReason()
{
    const testCase* tc = testGetCurrentCase();
    if(tc->status == TEST_CASE_COMPLETED)
    {
        assert(tc->abortReason);
        const int length = strlen(tc->abortReason);
        assert(length < TEST_MAX_MESSAGE_LENGTH);
        if(length > 0)
            return tc->abortReason;
    }

    return NULL;
}

int testCaseIsMarkedAsTodo()
{
    return testGetCurrentCase()->markedAsTodo;
}

const char* testGetCaseTodoReason()
{
    const testCase* tc = testGetCurrentCase();

    if(tc->markedAsTodo)
    {
        assert(tc->todoReason);
        const int length = strlen(tc->todoReason);
        assert(length < TEST_MAX_MESSAGE_LENGTH);
        if(length > 0)
            return tc->todoReason;
    }

    return NULL;
}

const char* testFormatV( const char* format, va_list args )
{
    static char buffer[TEST_MAX_MESSAGE_LENGTH];
    vsprintf(buffer, format, args);
    return buffer;
}

void testAbortCase( testAbortType type, const char* reason, ... )
{
    testCase* tc = testGetCurrentCase();
    switch(type)
    {
        case TEST_FAIL_CASE:
            tc->result = TEST_CASE_FAILED;
            break;

        case TEST_SKIP_CASE:
            tc->result = TEST_CASE_SKIPPED;
            break;

        default:
            assert(!"Unknown abort type");
    }
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        strncpy(tc->abortReason, testFormatV(reason, args), TEST_MAX_MESSAGE_LENGTH);
        va_end(args);
    }
    abort();
}

void testMarkCaseAsTodo( const char* reason, ... )
{
    testCase* tc = testGetCurrentCase();
    tc->markedAsTodo = true;
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        strncpy(tc->todoReason, testFormatV(reason, args), TEST_MAX_MESSAGE_LENGTH);
        va_end(args);
    }
}


// ----- protected call -----

testEnvironment* testGetCurrentEnvironment()
{
    testContext* ctx = testCurrentContext;
    assert(ctx);

    if(ctx->environmentStackSize > 0)
        return &ctx->environmentStack[ctx->environmentStackSize-1];
    else
        return NULL;
}

testEnvironment* testPushEnvironment()
{
    testContext* ctx = testCurrentContext;
    assert(ctx);

    ctx->environmentStackSize++;
    assert(ctx->environmentStackSize <= TEST_MAX_ENVIRONMENTS);

    testEnvironment* environment = testGetCurrentEnvironment();
    memset(environment, 0, sizeof(testEnvironment));
    return environment;
}

testEnvironment* testPopEnvironment()
{
    testContext* ctx = testCurrentContext;
    assert(ctx);

    assert(ctx->environmentStackSize > 0);
    testEnvironment* environment = testGetCurrentEnvironment();
    ctx->environmentStackSize--;
    return environment;
}

void testSignalHandler( int signal )
{
    testEnvironment* environment = testGetCurrentEnvironment();
    if(environment)
        longjmp(environment->jumpBuffer, 1);
}

bool testProtectedCall( testCaseFunction fn )
{
    testEnvironment* environment = testPushEnvironment();
    const int jumpResult = setjmp(environment->jumpBuffer);
    if(jumpResult == 0)
    {
        fn();
    }
    else
    {
        // fn aborted
    }
    testPopEnvironment();
    return jumpResult == 0;
}
