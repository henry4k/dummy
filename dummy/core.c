#include <stdlib.h> // malloc, free
#include <string.h> // memset, strncpy
#include <stdio.h>  // lamerFormat etc
#include <stdarg.h> // lamerFormat etc
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <assert.h>
#include "core.h"


enum
{
    LAMER_MAX_MESSAGE_LENGTH = 127,
    LAMER_MAX_CLEANUPS = 32,
    LAMER_MAX_TESTS = 32,
    LAMER_MAX_ENVIRONMENTS = 16,
    LAMER_INVALID_TEST_INDEX = -1
};


typedef void (*lamerCleanupFunction)( void* data );

/**
 * Is called after the test has been completed -
 * regardless of whether it failed or succeeded.
 */
typedef struct
{
    lamerCleanupFunction fn;
    void* data;
} lamerCleanup;

typedef struct
{
    char name[LAMER_MAX_MESSAGE_LENGTH];
    lamerTestFunction fn;
    lamerTestStatus status;

    lamerTestResult result;
    char abortReason[LAMER_MAX_MESSAGE_LENGTH];

    bool markedAsTodo;
    char todoReason[LAMER_MAX_MESSAGE_LENGTH];

    lamerCleanup cleanupStack[LAMER_MAX_CLEANUPS];
    int cleanupStackSize;
} lamerTest;

typedef struct
{
    jmp_buf jumpBuffer;
} lamerEnvironment;

typedef struct
{
    const lamerReporter* reporter;
    lamerStatus status;

    lamerTest tests[LAMER_MAX_TESTS];
    int testCount;

    int currentTestIndex;

    lamerEnvironment environmentStack[LAMER_MAX_ENVIRONMENTS];
    int environmentStackSize;
} lamerContext;


lamerContext* lamerCurrentContext = NULL;


bool lamerRunTest( int index );
bool lamerProtectedCall( lamerTestFunction fn );
void lamerSignalHandler( int signal );

void lamerInit( const lamerReporter* reporter )
{
    assert(lamerCurrentContext == NULL);
    lamerCurrentContext = malloc(sizeof(lamerContext));
    memset(lamerCurrentContext, 0, sizeof(lamerContext));

    assert(reporter);
    assert(reporter->began);
    assert(reporter->completed);
    assert(reporter->beganTest);
    assert(reporter->completedTest);
    assert(reporter->diag);
    lamerCurrentContext->reporter = reporter;

    lamerCurrentContext->status = LAMER_INITIALIZING;
    lamerCurrentContext->currentTestIndex = LAMER_INVALID_TEST_INDEX;
}

int lamerRunTests()
{
    lamerContext* ctx = lamerCurrentContext;
    assert(ctx);
    assert(ctx->status == LAMER_INITIALIZING);

    ctx->reporter->began(ctx->reporter->context);

    ctx->status = LAMER_RUNNING;
    int failedTests = 0;
    int i = 0;
    for(; i < ctx->testCount; i++)
        if(lamerRunTest(i) == false)
            failedTests++;

    ctx->status = LAMER_COMPLETED;
    ctx->reporter->completed(ctx->reporter->context);

    assert(ctx->status == LAMER_COMPLETED);
    free(ctx);
    lamerCurrentContext = NULL;

    return failedTests;
}

int lamerAddTest( const char* name, lamerTestFunction fn )
{
    lamerContext* ctx = lamerCurrentContext;
    assert(ctx);
    assert(ctx->status == LAMER_INITIALIZING);
    assert(ctx->testCount < LAMER_MAX_TESTS);

    lamerTest* tc = &ctx->tests[ctx->testCount];
    memset(tc, 0, sizeof(lamerTest));

    strncpy(tc->name, name, LAMER_MAX_MESSAGE_LENGTH);
    tc->fn = fn;

    ctx->testCount++;

    return ctx->testCount-1;
}

bool lamerRunTest( int index )
{
    lamerContext* ctx = lamerCurrentContext;
    assert(ctx);
    assert(ctx->status == LAMER_RUNNING);

    assert(index >= 0);
    assert(index < LAMER_MAX_TESTS);
    lamerTest* tc = &ctx->tests[index];

    // prepare
    ctx->currentTestIndex = index;
    tc->status = LAMER_TEST_STARTING;
    tc->result = LAMER_TEST_PASSED;
    ctx->reporter->beganTest(ctx->reporter->context);
    signal(SIGABRT, lamerSignalHandler);
    signal(SIGFPE, lamerSignalHandler);
    signal(SIGILL, lamerSignalHandler);
    signal(SIGSEGV, lamerSignalHandler);

    // run
    tc->status = LAMER_TEST_RUNNING;
    const bool passed = lamerProtectedCall(tc->fn);
    if(!passed && tc->result == LAMER_TEST_PASSED)
        tc->result = LAMER_TEST_FAILED;
    tc->status = LAMER_TEST_COMPLETED;

    // cleanup
    signal(SIGABRT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    ctx->reporter->completedTest(ctx->reporter->context);
    ctx->currentTestIndex = LAMER_INVALID_TEST_INDEX;
    tc->status = LAMER_TEST_UNDEFINED;

    return passed;
}

lamerStatus lamerGetStatus()
{
    lamerContext* ctx = lamerCurrentContext;
    assert(ctx);

    return ctx->status;
}

int lamerGetTestCount()
{
    const lamerContext* ctx = lamerCurrentContext;
    assert(ctx);

    return ctx->testCount;
}

int lamerGetTestNumber()
{
    const lamerContext* ctx = lamerCurrentContext;
    assert(ctx);

    assert(ctx->currentTestIndex >= 0);
    assert(ctx->currentTestIndex < LAMER_MAX_TESTS);

    return ctx->currentTestIndex;
}

lamerTest* lamerGetCurrentTest()
{
    lamerContext* ctx = lamerCurrentContext;
    assert(ctx);

    assert(ctx->currentTestIndex >= 0);
    assert(ctx->currentTestIndex < LAMER_MAX_TESTS);
    lamerTest* tc = &ctx->tests[ctx->currentTestIndex];

    return tc;
}

const char* lamerGetTestName()
{
    const lamerTest* tc = lamerGetCurrentTest();
    assert(tc->name);
    assert(strlen(tc->name) < LAMER_MAX_MESSAGE_LENGTH);
    return tc->name;
}

lamerTestStatus lamerGetTestStatus()
{
    return lamerGetCurrentTest()->status;
}

lamerTestResult lamerGetTestResult()
{
    const lamerTest* tc = lamerGetCurrentTest();
    assert(tc->status == LAMER_TEST_COMPLETED);
    return tc->result;
}

const char* lamerGetTestAbortReason()
{
    const lamerTest* tc = lamerGetCurrentTest();
    if(tc->status == LAMER_TEST_COMPLETED)
    {
        assert(tc->abortReason);
        const int length = strlen(tc->abortReason);
        assert(length < LAMER_MAX_MESSAGE_LENGTH);
        if(length > 0)
            return tc->abortReason;
    }

    return NULL;
}

int lamerTestIsMarkedAsTodo()
{
    return lamerGetCurrentTest()->markedAsTodo;
}

const char* lamerGetTestTodoReason()
{
    const lamerTest* tc = lamerGetCurrentTest();

    if(tc->markedAsTodo)
    {
        assert(tc->todoReason);
        const int length = strlen(tc->todoReason);
        assert(length < LAMER_MAX_MESSAGE_LENGTH);
        if(length > 0)
            return tc->todoReason;
    }

    return NULL;
}

const char* lamerFormatV( const char* format, va_list args )
{
    static char buffer[LAMER_MAX_MESSAGE_LENGTH];
    vsprintf(buffer, format, args);
    return buffer;
}

void lamerAbortTest( lamerTestAbortType type, const char* reason, ... )
{
    lamerTest* tc = lamerGetCurrentTest();
    switch(type)
    {
        case LAMER_FAIL_TEST:
            tc->result = LAMER_TEST_FAILED;
            break;

        case LAMER_SKIP_TEST:
            tc->result = LAMER_TEST_SKIPPED;
            break;

        default:
            assert(!"Unknown abort type");
    }
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        strncpy(tc->abortReason, lamerFormatV(reason, args), LAMER_MAX_MESSAGE_LENGTH);
        va_end(args);
    }
    abort();
}

void lamerMarkTestAsTodo( const char* reason, ... )
{
    lamerTest* tc = lamerGetCurrentTest();
    tc->markedAsTodo = true;
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        strncpy(tc->todoReason, lamerFormatV(reason, args), LAMER_MAX_MESSAGE_LENGTH);
        va_end(args);
    }
}


// ----- protected call -----

lamerEnvironment* lamerGetCurrentEnvironment()
{
    lamerContext* ctx = lamerCurrentContext;
    assert(ctx);

    if(ctx->environmentStackSize > 0)
        return &ctx->environmentStack[ctx->environmentStackSize-1];
    else
        return NULL;
}

lamerEnvironment* lamerPushEnvironment()
{
    lamerContext* ctx = lamerCurrentContext;
    assert(ctx);

    ctx->environmentStackSize++;
    assert(ctx->environmentStackSize <= LAMER_MAX_ENVIRONMENTS);

    lamerEnvironment* environment = lamerGetCurrentEnvironment();
    memset(environment, 0, sizeof(lamerEnvironment));
    return environment;
}

lamerEnvironment* lamerPopEnvironment()
{
    lamerContext* ctx = lamerCurrentContext;
    assert(ctx);

    assert(ctx->environmentStackSize > 0);
    lamerEnvironment* environment = lamerGetCurrentEnvironment();
    ctx->environmentStackSize--;
    return environment;
}

void lamerSignalHandler( int signal )
{
    lamerEnvironment* environment = lamerGetCurrentEnvironment();
    if(environment)
        longjmp(environment->jumpBuffer, 1);
}

bool lamerProtectedCall( lamerTestFunction fn )
{
    lamerEnvironment* environment = lamerPushEnvironment();
    const int jumpResult = setjmp(environment->jumpBuffer);
    if(jumpResult == 0)
    {
        fn();
    }
    else
    {
        // fn aborted
    }
    lamerPopEnvironment();
    return jumpResult == 0;
}
