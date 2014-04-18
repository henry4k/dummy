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
    fprintf(ctx->file, "<lamersuite lamers=\"%d\">%s",
        lamerGetTestCount(),
        ctx->newline
    );
}

static void completed( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "</lamersuite>%s",
        ctx->newline
    );
}

static void beganTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;
    fprintf(ctx->file, "%s<lamertest name=\"%s\">%s",
        ctx->tab,
        lamerGetTestName(),
        ctx->newline
    );
}

static void completedTest( void* ctx_ )
{
    const Context* ctx = (Context*)ctx_;

    const char* abortReason = lamerGetTestAbortReason();

    lamerTestResult result = lamerGetTestResult();
    switch(result)
    {
        case LAMER_TEST_PASSED:
            break;

        case LAMER_TEST_FAILED:
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

        case LAMER_TEST_SKIPPED:
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

    fprintf(ctx->file, "%s</lamertest>%s",
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

const lamerReporter* lamerGetXMLReporter()
{
    static Context ctx;
    static lamerReporter reporter;

    ctx.file = stdout;
    ctx.tab = "    ";
    ctx.newline = "\n";

    reporter.context = &ctx;
    reporter.began = began;
    reporter.completed = completed;
    reporter.beganTest = beganTest;
    reporter.completedTest = completedTest;
    reporter.diag = diag;

    return &reporter;
}
