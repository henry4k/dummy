#include <assert.h>
#include <stdio.h>

#include "config.h"
#include "sandbox.h"


typedef struct
{
    dummyAbortHandler handler;
    void* context;
} AbortHandlerInfo;


static AbortHandlerInfo abortStack[DUMMY_MAX_SANDBOX_DEPTH];
static int abortStackSize = 0;

void dummyPushAbortHandler( dummyAbortHandler handler, void* context )
{
    assert(abortStackSize <= DUMMY_MAX_SANDBOX_DEPTH);
    abortStackSize++;
    AbortHandlerInfo* info = &abortStack[abortStackSize-1];

    info->handler = handler;
    info->context = context;
}

void dummyPopAbortHandler()
{
    assert(abortStackSize >= 1);
    abortStackSize--;
}

void dummyAbortSandbox( int errorCode, const char* reason )
{
    assert(abortStackSize >= 1);
    AbortHandlerInfo* info = &abortStack[abortStackSize-1];

    info->handler(info->context, errorCode, reason);
}
