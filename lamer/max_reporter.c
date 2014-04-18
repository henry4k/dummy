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
    fprintf(ctx->file, "test count = %d\n",
        lamerGetTestCount()
    );
}

static void completed( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "testing done\n");
}

static void beganTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "test number = %d\n",
        lamerGetTestNumber()
    );
    fprintf(ctx->file, "test name = '%s'\n",
        lamerGetTestName()
    );
}

static void completedTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    const char* result = NULL;
    switch(lamerGetTestResult())
    {
        case LAMER_TEST_PASSED:
            result = "passed";
            break;

        case LAMER_TEST_FAILED:
            result = "failed";
            break;

        case LAMER_TEST_SKIPPED:
            result = "skipped";
            break;

        default:
            assert(!"Unknown result");
    }
    fprintf(ctx->file, "test result = '%s'\n",
        result
    );
    fprintf(ctx->file, "test abort reason = '%s'\n",
        lamerGetTestAbortReason()
    );

    fprintf(ctx->file, "test todo = %d\n",
        lamerTestIsMarkedAsTodo()
    );
    fprintf(ctx->file, "test todo reason = '%s'\n",
        lamerGetTestTodoReason()
    );

    fprintf(ctx->file, "test done\n");
}

static void diag( void* ctx_, const char* message )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "%s\n",
        message
    );
}

const lamerReporter* lamerGetMaxReporter()
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
