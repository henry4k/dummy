#include <stdlib.h> // malloc, free
#include <string.h> // memset, strncpy
#include <stdio.h>  // dummyFormat etc
#include <stdarg.h> // dummyFormat etc
#include <stdbool.h>
#include <assert.h>

#include "protected_call.h"
#include "core.h"


enum
{
    DUMMY_MAX_MESSAGE_LENGTH = 127,
    DUMMY_MAX_TESTS = 32,
    DUMMY_INVALID_TEST_INDEX = -1,
    DUMMY_PROTECTED_CALL_SKIPPED = -1
};

typedef struct
{
    char name[DUMMY_MAX_MESSAGE_LENGTH];
    dummyTestFunction fn;
    dummyTestStatus status;

    dummyTestResult result;
    char abortReason[DUMMY_MAX_MESSAGE_LENGTH];

    bool markedAsTodo;
    char todoReason[DUMMY_MAX_MESSAGE_LENGTH];
} dummyTest;

typedef struct
{
    const dummyReporter* reporter;
    dummyStatus status;

    dummyTest tests[DUMMY_MAX_TESTS];
    int testCount;

    int currentTestIndex;
} dummyContext;


dummyContext* dummyCurrentContext = NULL;


bool dummyRunTest( int index );
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

    // run
    test->status = DUMMY_TEST_RUNNING;
    const char* abortReason = NULL;
    const int errorCode = dummyProtectedCall(test->fn, &abortReason);
    switch(errorCode)
    {
        case DUMMY_PROTECTED_CALL_SUCEEDED:
            test->result = DUMMY_TEST_PASSED;
            break;

        case DUMMY_PROTECTED_CALL_SKIPPED:
            test->result = DUMMY_TEST_SKIPPED;
            break;

        case DUMMY_PROTECTED_CALL_GENERIC_ERROR:
        default:
            test->result = DUMMY_TEST_FAILED;
    }
    if(abortReason)
        strncpy(test->abortReason, abortReason, DUMMY_MAX_MESSAGE_LENGTH);
    test->status = DUMMY_TEST_COMPLETED;

    // cleanup
    ctx->reporter->completedTest(ctx->reporter->context);
    ctx->currentTestIndex = DUMMY_INVALID_TEST_INDEX;
    test->status = DUMMY_TEST_UNDEFINED;

    return test->result != DUMMY_TEST_FAILED;
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
    int errorCode = DUMMY_PROTECTED_CALL_GENERIC_ERROR;
    switch(type)
    {
        case DUMMY_FAIL_TEST:
            errorCode = DUMMY_PROTECTED_CALL_GENERIC_ERROR;
            break;

        case DUMMY_SKIP_TEST:
            errorCode = DUMMY_PROTECTED_CALL_SKIPPED;
            break;

        default:
            assert(!"Unknown abort type");
    }

    const char* formattedReason = NULL;
    if(reason)
    {
        va_list args;
        va_start(args, reason);
        formattedReason = dummyFormatV(reason, args);
        va_end(args);
    }

    dummyAbortProtectedCall(errorCode, formattedReason);
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
