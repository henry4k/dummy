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

typedef void (*dummyProtectableFunction)();


dummyContext* dummyCurrentContext = NULL;


bool dummyRunTest( int index );
bool dummyProtectedCall( dummyTestFunction fn );
void dummySignalHandler( int signal );
dummyTest* dummyGetCurrentTest();

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
    assert(reporter->log);
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

    dummyTest* test = &ctx->tests[ctx->testCount];
    memset(test, 0, sizeof(dummyTest));

    strncpy(test->name, name, DUMMY_MAX_MESSAGE_LENGTH);
    test->fn = fn;

    ctx->testCount++;

    return ctx->testCount-1;
}

void dummyAddCleanup( dummyCleanupFunction fn, void* data )
{
    dummyTest* test = dummyGetCurrentTest();
    assert(test->cleanupStackSize < DUMMY_MAX_CLEANUPS);
    dummyCleanup* cleanup = &test->cleanupStack[test->cleanupStackSize];

    cleanup->fn = fn;
    cleanup->data = data;

    test->cleanupStackSize++;
}

void dummyRunTestCallback()
{
    dummyTest* test = dummyGetCurrentTest();

    test->fn();

    for(int i = 0; i < test->cleanupStackSize; i++)
    {
        dummyCleanup* cleanup = &test->cleanupStack[i];
        cleanup->fn(cleanup->data);
    }
    test->cleanupStackSize = 0;
}

bool dummyRunTest( int index )
{
    dummyContext* ctx = dummyCurrentContext;
    assert(ctx);
    assert(ctx->status == DUMMY_RUNNING);

    assert(index >= 0);
    assert(index < DUMMY_MAX_TESTS);
    dummyTest* test = &ctx->tests[index];

    // prepare
    ctx->currentTestIndex = index;
    test->status = DUMMY_TEST_STARTING;
    test->result = DUMMY_TEST_PASSED;
    ctx->reporter->beganTest(ctx->reporter->context);
    signal(SIGABRT, dummySignalHandler);
    signal(SIGFPE, dummySignalHandler);
    signal(SIGILL, dummySignalHandler);
    signal(SIGSEGV, dummySignalHandler);

    // run
    test->status = DUMMY_TEST_RUNNING;
    const bool passed = dummyProtectedCall(dummyRunTestCallback);
    if(!passed && test->result == DUMMY_TEST_PASSED)
        test->result = DUMMY_TEST_FAILED;
    test->status = DUMMY_TEST_COMPLETED;

    // cleanup
    signal(SIGABRT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    ctx->reporter->completedTest(ctx->reporter->context);
    ctx->currentTestIndex = DUMMY_INVALID_TEST_INDEX;
    test->status = DUMMY_TEST_UNDEFINED;

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
    dummyTest* test = &ctx->tests[ctx->currentTestIndex];

    return test;
}

const char* dummyGetTestName()
{
    const dummyTest* test = dummyGetCurrentTest();
    assert(test->name);
    assert(strlen(test->name) < DUMMY_MAX_MESSAGE_LENGTH);
    return test->name;
}

dummyTestStatus dummyGetTestStatus()
{
    return dummyGetCurrentTest()->status;
}

dummyTestResult dummyGetTestResult()
{
    const dummyTest* test = dummyGetCurrentTest();
    assert(test->status == DUMMY_TEST_COMPLETED);
    return test->result;
}

const char* dummyGetTestAbortReason()
{
    const dummyTest* test = dummyGetCurrentTest();
    if(test->status == DUMMY_TEST_COMPLETED)
    {
        assert(test->abortReason);
        const int length = strlen(test->abortReason);
        assert(length < DUMMY_MAX_MESSAGE_LENGTH);
        if(length > 0)
            return test->abortReason;
    }

    return NULL;
}

int dummyTestIsMarkedAsTodo()
{
    return dummyGetCurrentTest()->markedAsTodo;
}

const char* dummyGetTestTodoReason()
{
    const dummyTest* test = dummyGetCurrentTest();

    if(test->markedAsTodo)
    {
        assert(test->todoReason);
        const int length = strlen(test->todoReason);
        assert(length < DUMMY_MAX_MESSAGE_LENGTH);
        if(length > 0)
            return test->todoReason;
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
    dummyTest* test = dummyGetCurrentTest();
    switch(type)
    {
        case DUMMY_FAIL_TEST:
            test->result = DUMMY_TEST_FAILED;
            break;

        case DUMMY_SKIP_TEST:
            test->result = DUMMY_TEST_SKIPPED;
            break;

        default:
            assert(!"Unknown abort type");
    }
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        strncpy(test->abortReason, dummyFormatV(reason, args), DUMMY_MAX_MESSAGE_LENGTH);
        va_end(args);
    }
    abort();
}

void dummyMarkTestAsTodo( const char* reason, ... )
{
    dummyTest* test = dummyGetCurrentTest();
    test->markedAsTodo = true;
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        strncpy(test->todoReason, dummyFormatV(reason, args), DUMMY_MAX_MESSAGE_LENGTH);
        va_end(args);
    }
}

void dummyLog( const char* message, ... )
{
    const dummyContext* ctx = dummyCurrentContext;
    assert(ctx);

    va_list args;
    va_start(args, message);
    const char* formattedMessage = dummyFormatV(message, args);
    va_end(args);

    ctx->reporter->log(ctx->reporter->context, formattedMessage);
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

bool dummyProtectedCall( dummyProtectableFunction fn )
{
    dummyEnvironment* environment = dummyPushEnvironment();
    const int jumpResult = setjmp(environment->jumpBuffer);
    if(jumpResult == 0)
        fn();
    dummyPopEnvironment();
    return jumpResult == 0;
}
