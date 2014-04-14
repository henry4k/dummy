#include <stdio.h>
#include <assert.h>
#include "max_reporter.h"

typedef struct
{
    FILE* file;
} Context;

static void began( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "test case count = %d\n",
        testGetCaseCount()
    );
}

static void completed( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "testing done\n");
}

static void beganCase( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "test case number = %d\n",
        testGetCaseNumber()
    );
    fprintf(ctx->file, "test case name = '%s'\n",
        testGetCaseName()
    );
}

static void completedCase( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    const char* result = NULL;
    switch(testGetCaseResult())
    {
        case TEST_CASE_PASSED:
            result = "passed";
            break;

        case TEST_CASE_FAILED:
            result = "failed";
            break;

        case TEST_CASE_SKIPPED:
            result = "skipped";
            break;

        default:
            assert(!"Unknown result");
    }
    fprintf(ctx->file, "test case result = '%s'\n",
        result
    );
    fprintf(ctx->file, "test case abort reason = '%s'\n",
        testGetCaseAbortReason()
    );

    fprintf(ctx->file, "test case todo = %d\n",
        testCaseIsMarkedAsTodo()
    );
    fprintf(ctx->file, "test case todo reason = '%s'\n",
        testGetCaseTodoReason()
    );

    fprintf(ctx->file, "test case done\n");
}

static void diag( void* ctx_, const char* message )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "%s\n",
        message
    );
}

const testReporter* testGetMaxReporter()
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
