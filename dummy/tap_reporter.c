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
    fprintf(ctx->file, "1..%d\n", lamerGetTestCount());
}

static void completed( void* ctx_ )
{
}

static void beganTest( void* ctx_ )
{
}

static const char* lamerTestResultToTAPString( lamerTestResult result )
{
    switch(result)
    {
        case LAMER_TEST_PASSED:
            return "ok";
        case LAMER_TEST_FAILED:
            return "not ok";
        case LAMER_TEST_SKIPPED:
            return "ok";
        default:
            assert(!"Unknown result");
    }
}

static void completedTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    lamerTestResult result = lamerGetTestResult();
    const char* resultString = lamerTestResultToTAPString(result);
    const char* abortReason = lamerGetTestAbortReason();

    if(result == LAMER_TEST_FAILED && abortReason)
    {
        fprintf(ctx->file, "# %s\n", abortReason);
    }

    fprintf(ctx->file, "%s %d %s",
        resultString,
        lamerGetTestNumber()+1,
        lamerGetTestName()
    );

    if(lamerTestIsMarkedAsTodo())
    {
        fprintf(ctx->file, " # TODO");
        const char* todoReason = lamerGetTestTodoReason();
        if(todoReason)
            fprintf(ctx->file, " %s", todoReason);
    }
    else if(result == LAMER_TEST_SKIPPED)
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

const lamerReporter* lamerGetTAPReporter()
{
    static Context ctx;
    static lamerReporter reporter;

    ctx.file = stdout;

    reporter.context = &ctx;
    reporter.began = began;
    reporter.completed = completed;
    reporter.beganTest = beganTest;
    reporter.completedTest = completedTest;
    reporter.diag = diag;

    return &reporter;
}
