#include <stdio.h>
#include <assert.h>
#include "xml_reporter.h"

typedef struct
{
    FILE* file;
    const char* tab;
    const char* newline;
} Context;

static void began( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "<testsuite tests=\"%d\">%s",
        testGetCaseCount(),
        ctx->newline
    );
}

static void completed( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "</testsuite>%s",
        ctx->newline
    );
}

static void beganCase( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "%s<testcase name=\"%s\">%s",
        ctx->tab,
        testGetCaseName(),
        ctx->newline
    );
}

static void completedCase( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    const char* abortReason = testGetCaseAbortReason();

    testCaseResult result = testGetCaseResult();
    switch(result)
    {
        case TEST_CASE_PASSED:
            break;

        case TEST_CASE_FAILED:
            if(abortReason)
            {
                fprintf(ctx->file, "%s%s<failure>%s</failure>%s",
                    ctx->tab,
                    ctx->tab,
                    abortReason,
                    ctx->newline
                );
            }
            else
            {
                fprintf(ctx->file, "%s%s<failure/>%s",
                    ctx->tab,
                    ctx->tab,
                    ctx->newline
                );
            }
            break;

        case TEST_CASE_SKIPPED:
            if(abortReason)
            {
                fprintf(ctx->file, "%s%s<skipped>%s</failure>%s",
                    ctx->tab,
                    ctx->tab,
                    abortReason,
                    ctx->newline
                );
            }
            else
            {
                fprintf(ctx->file, "%s%s<skipped/>%s",
                    ctx->tab,
                    ctx->tab,
                    ctx->newline
                );
            }
            break;

        default:
            assert(!"Unknown result");
    }

    fprintf(ctx->file, "%s</testcase>%s",
        ctx->tab,
        ctx->newline
    );
}

static void diag( void* ctx_, const char* message )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "%s%s<system-out>%s</system-out>%s",
        ctx->tab,
        ctx->tab,
        message,
        ctx->newline
    );
}

const testReporter* testGetXMLReporter()
{
    static Context ctx;
    static testReporter reporter;

    ctx.file = stdout;
    ctx.tab = "    ";
    ctx.newline = "\n";

    reporter.context = &ctx;
    reporter.began = began;
    reporter.completed = completed;
    reporter.beganCase = beganCase;
    reporter.completedCase = completedCase;
    reporter.diag = diag;

    return &reporter;
}
