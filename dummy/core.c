#include <stdlib.h> // malloc, free
#include <string.h> // memset, strncpy
#include <stdio.h>  // dummyFormat etc
#include <stdarg.h> // dummyFormat etc
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <assert.h>
#include "core.h"


enum
{
    DUMMY_MAX_MESSAGE_LENGTH = 127,
    DUMMY_MAX_CLEANUPS = 32,
    DUMMY_MAX_TESTS = 32,
    DUMMY_MAX_ENVIRONMENTS = 16,
    DUMMY_INVALID_TEST_INDEX = -1
};


typedef void (*dummyCleanupFunction)( void* data );

/**
 * Is called after the test has been completed -
 * regardless of whether it failed or succeeded.
 */
typedef struct
{
    dummyCleanupFunction fn;
    void* data;
} dummyCleanup;

typedef struct
{
    char name[DUMMY_MAX_MESSAGE_LENGTH];
    dummyTestFunction fn;
    dummyTestStatus status;

    dummyTestResult result;
    char abortReason[DUMMY_MAX_MESSAGE_LENGTH];

    bool markedAsTodo;
    char todoReason[DUMMY_MAX_MESSAGE_LENGTH];

    dummyCleanup cleanupStack[DUMMY_MAX_CLEANUPS];
    int cleanupStackSize;
} dummyTest;

typedef struct
{
    jmp_buf jumpBuffer;
} dummyEnvironment;

typedef struct
{
    const dummyReporter* reporter;
    dummyStatus status;

    dummyTest tests[DUMMY_MAX_TESTS];
    int testCount;

    int currentTestIndex;

    dummyEnvironment environmentStack[DUMMY_MAX_ENVIRONMENTS];
    int environmentStackSize;
} dummyContext;


dummyContext* dummyCurrentContext = NULL;


bool dummyRunTest( int index );
bool dummyProtectedCall( dummyTestFunction fn );
void dummySignalHandler( int signal );

void dummyInit( const dummyReporter* reporter )
{
    assert(dummyCurrentContext == NULL);
    dummyCurrentContext = malloc(sizeof(dummyContext));
    memset(dummyCurrentContext, 0, sizeof(dummyContext));

    assert(reporter);
    assert(reporter->began);
    assert(reporter->completed);
    assert(reporter->beganTest);
    assert(reporter->completedTest);
    assert(reporter->diag);
    dummyCurrentContext->reporter = reporter;

    dummyCurrentContext->status = DUMMY_INITIALIZING;
    dummyCurrentContext->currentTestIndex = DUMMY_INVALID_TEST_INDEX;
}

int dummyRunTests()
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);
    assert(ctx->status == DUMMY_INITIALIZING);

    ctx->reporter->began(ctx->reporter->context);

    ctx->status = DUMMY_RUNNING;
    int failedTests = 0;
    int i = 0;
    for(; i < ctx->testCount; i++)
        if(dummyRunTest(i) == false)
            failedTests++;

    ctx->status = DUMMY_COMPLETED;
    ctx->reporter->completed(ctx->reporter->context);

    assert(ctx->status == DUMMY_COMPLETED);
    free(ctx);
    dummyCurrentContext = NULL;

    return failedTests;
}

int dummyAddTest( const char* name, dummyTestFunction fn )
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);
    assert(ctx->status == DUMMY_INITIALIZING);
    assert(ctx->testCount < DUMMY_MAX_TESTS);

    dummyTest* tc = &ctx->tests[ctx->testCount];
    memset(tc, 0, sizeof(dummyTest));

    strncpy(tc->name, name, DUMMY_MAX_MESSAGE_LENGTH);
    tc->fn = fn;

    ctx->testCount++;

    return ctx->testCount-1;
}

bool dummyRunTest( int index )
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);
    assert(ctx->status == DUMMY_RUNNING);

    assert(index >= 0);
    assert(index < DUMMY_MAX_TESTS);
    dummyTest* tc = &ctx->tests[index];

    // prepare
    ctx->currentTestIndex = index;
    tc->status = DUMMY_TEST_STARTING;
    tc->result = DUMMY_TEST_PASSED;
    ctx->reporter->beganTest(ctx->reporter->context);
    signal(SIGABRT, dummySignalHandler);
    signal(SIGFPE, dummySignalHandler);
    signal(SIGILL, dummySignalHandler);
    signal(SIGSEGV, dummySignalHandler);

    // run
    tc->status = DUMMY_TEST_RUNNING;
    const bool passed = dummyProtectedCall(tc->fn);
    if(!passed && tc->result == DUMMY_TEST_PASSED)
        tc->result = DUMMY_TEST_FAILED;
    tc->status = DUMMY_TEST_COMPLETED;

    // cleanup
    signal(SIGABRT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    ctx->reporter->completedTest(ctx->reporter->context);
    ctx->currentTestIndex = DUMMY_INVALID_TEST_INDEX;
    tc->status = DUMMY_TEST_UNDEFINED;

    return passed;
}

dummyStatus dummyGetStatus()
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);

    return ctx->status;
}

int dummyGetTestCount()
{
    const dummyContext* ctx = dummyCurrentContext;
    assert(ctx);

    return ctx->testCount;
}

int dummyGetTestNumber()
{
    const dummyContext* ctx = dummyCurrentContext;
    assert(ctx);

    assert(ctx->currentTestIndex >= 0);
    assert(ctx->currentTestIndex < DUMMY_MAX_TESTS);

    return ctx->currentTestIndex;
}

dummyTest* dummyGetCurrentTest()
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);

    assert(ctx->currentTestIndex >= 0);
    assert(ctx->currentTestIndex < DUMMY_MAX_TESTS);
    dummyTest* tc = &ctx->tests[ctx->currentTestIndex];

    return tc;
}

const char* dummyGetTestName()
{
    const dummyTest* tc = dummyGetCurrentTest();
    assert(tc->name);
    assert(strlen(tc->name) < DUMMY_MAX_MESSAGE_LENGTH);
    return tc->name;
}

dummyTestStatus dummyGetTestStatus()
{
    return dummyGetCurrentTest()->status;
}

dummyTestResult dummyGetTestResult()
{
    const dummyTest* tc = dummyGetCurrentTest();
    assert(tc->status == DUMMY_TEST_COMPLETED);
    return tc->result;
}

const char* dummyGetTestAbortReason()
{
    const dummyTest* tc = dummyGetCurrentTest();
    if(tc->status == DUMMY_TEST_COMPLETED)
    {
        assert(tc->abortReason);
        const int length = strlen(tc->abortReason);
        assert(length < DUMMY_MAX_MESSAGE_LENGTH);
        if(length > 0)
            return tc->abortReason;
    }

    return NULL;
}

int dummyTestIsMarkedAsTodo()
{
    return dummyGetCurrentTest()->markedAsTodo;
}

const char* dummyGetTestTodoReason()
{
    const dummyTest* tc = dummyGetCurrentTest();

    if(tc->markedAsTodo)
    {
        assert(tc->todoReason);
        const int length = strlen(tc->todoReason);
        assert(length < DUMMY_MAX_MESSAGE_LENGTH);
        if(length > 0)
            return tc->todoReason;
    }

    return NULL;
}

const char* dummyFormatV( const char* format, va_list args )
{
    static char buffer[DUMMY_MAX_MESSAGE_LENGTH];
    vsprintf(buffer, format, args);
    return buffer;
}

void dummyAbortTest( dummyTestAbortType type, const char* reason, ... )
{
    dummyTest* tc = dummyGetCurrentTest();
    switch(type)
    {
        case DUMMY_FAIL_TEST:
            tc->result = DUMMY_TEST_FAILED;
            break;

        case DUMMY_SKIP_TEST:
            tc->result = DUMMY_TEST_SKIPPED;
            break;

        default:
            assert(!"Unknown abort type");
    }
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        strncpy(tc->abortReason, dummyFormatV(reason, args), DUMMY_MAX_MESSAGE_LENGTH);
        va_end(args);
    }
    abort();
}

void dummyMarkTestAsTodo( const char* reason, ... )
{
    dummyTest* tc = dummyGetCurrentTest();
    tc->markedAsTodo = true;
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        strncpy(tc->todoReason, dummyFormatV(reason, args), DUMMY_MAX_MESSAGE_LENGTH);
        va_end(args);
    }
}


// ----- protected call -----

dummyEnvironment* dummyGetCurrentEnvironment()
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);

    if(ctx->environmentStackSize > 0)
        return &ctx->environmentStack[ctx->environmentStackSize-1];
    else
        return NULL;
}

dummyEnvironment* dummyPushEnvironment()
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);

    ctx->environmentStackSize++;
    assert(ctx->environmentStackSize <= DUMMY_MAX_ENVIRONMENTS);

    dummyEnvironment* environment = dummyGetCurrentEnvironment();
    memset(environment, 0, sizeof(dummyEnvironment));
    return environment;
}

dummyEnvironment* dummyPopEnvironment()
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);

    assert(ctx->environmentStackSize > 0);
    dummyEnvironment* environment = dummyGetCurrentEnvironment();
    ctx->environmentStackSize--;
    return environment;
}

void dummySignalHandler( int signal )
{
    dummyEnvironment* environment = dummyGetCurrentEnvironment();
    if(environment)
        longjmp(environment->jumpBuffer, 1);
}

bool dummyProtectedCall( dummyTestFunction fn )
{
    dummyEnvironment* environment = dummyPushEnvironment();
    const int jumpResult = setjmp(environment->jumpBuffer);
    if(jumpResult == 0)
    {
        fn();
    }
    else
    {
        // fn aborted
    }
    dummyPopEnvironment();
    return jumpResult == 0;
}
