#include <stdio.h>
#include <assert.h>
#include "tap_reporter.h"

typedef struct
{
    FILE* file;
} Context;

static void began( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "1..%d\n", testGetCaseCount());
}

static void completed( void* ctx_ )
{
}

static void beganCase( void* ctx_ )
{
}

static const char* testCaseResultToTAPString( testCaseResult result )
{
    switch(result)
    {
        case TEST_CASE_PASSED:
            return "ok";
        case TEST_CASE_FAILED:
            return "not ok";
        case TEST_CASE_SKIPPED:
            return "ok";
        default:
            assert(!"Unknown result");
    }
}

static void completedCase( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    testCaseResult result = testGetCaseResult();
    const char* resultString = testCaseResultToTAPString(result);
    const char* abortReason = testGetCaseAbortReason();

    if(result == TEST_CASE_FAILED && abortReason)
    {
        fprintf(ctx->file, "# %s\n", abortReason);
    }

    fprintf(ctx->file, "%s %d %s",
        resultString,
        testGetCaseNumber()+1,
        testGetCaseName()
    );

    if(testCaseIsMarkedAsTodo())
    {
        fprintf(ctx->file, " # TODO");
        const char* todoReason = testGetCaseTodoReason();
        if(todoReason)
            fprintf(ctx->file, " %s", todoReason);
    }
    else if(result == TEST_CASE_SKIPPED)
    {
        fprintf(ctx->file, " # SKIP");
        if(abortReason)
            fprintf(ctx->file, " %s", abortReason);
    };

    fprintf(ctx->file, "\n");
}

static void diag( void* ctx_, const char* message )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "# %s\n", message);
}

const testReporter* testGetTAPReporter()
{
    static Context ctx;
    static testReporter reporter;

    ctx.file = stdout;

    reporter.context = &ctx;
    reporter.began = began;
    reporter.completed = completed;
    reporter.beganCase = beganCase;
    reporter.completedCase = completedCase;
    reporter.diag = diag;

    return &reporter;
}
