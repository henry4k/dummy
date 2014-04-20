#include <stddef.h> // NULL
#include <signal.h>
#include <setjmp.h>
#include <string.h> // memset, strncpy
#include <fenv.h> // floating point exceptions
#include <assert.h>

#include "protected_call.h"


enum
{
    DUMMY_MAX_ENVIRONMENTS = 16,
    DUMMY_MAX_PROTECTED_CALL_REASON_LENGTH = 512
};

typedef struct
{
    jmp_buf jumpBuffer;
    int errorCode;
    char abortReason[DUMMY_MAX_PROTECTED_CALL_REASON_LENGTH];
} dummyEnvironment;

typedef void (*dummySignalFunction)(int);


dummyEnvironment dummyEnvironmentStack[DUMMY_MAX_ENVIRONMENTS];
int dummyEnvironmentStackSize = 0;


void dummySignalHandler( int signal );
void dummySetAbortInformation( int errorCode, const char* reason );

void dummySetSignals( dummySignalFunction fn )
{
    signal(SIGABRT, fn);
    signal(SIGFPE, fn);
    signal(SIGILL, fn);
    signal(SIGSEGV, fn);
}

dummyEnvironment* dummyGetCurrentEnvironment()
{
    if(dummyEnvironmentStackSize > 0)
        return &dummyEnvironmentStack[dummyEnvironmentStackSize-1];
    else
        return NULL;
}

dummyEnvironment* dummyPushEnvironment()
{
    if(dummyEnvironmentStackSize == 0)
        dummySetSignals(dummySignalHandler);

    dummyEnvironmentStackSize++;
    assert(dummyEnvironmentStackSize <= DUMMY_MAX_ENVIRONMENTS);

    dummyEnvironment* environment = dummyGetCurrentEnvironment();
    memset(environment, 0, sizeof(dummyEnvironment));
    environment->errorCode = DUMMY_PROTECTED_CALL_SUCEEDED;
    return environment;
}

dummyEnvironment* dummyPopEnvironment()
{
    assert(dummyEnvironmentStackSize > 0);
    dummyEnvironment* environment = dummyGetCurrentEnvironment();
    dummyEnvironmentStackSize--;
    if(dummyEnvironmentStackSize == 0)
        dummySetSignals(SIG_DFL);
    return environment;
}

const char* dummySignalToAbortReason( int signal )
{
    switch(signal)
    {
        case SIGABRT:
            return "SIGABRT: Abnormal termination";

        case SIGILL:
            return "SIGILL: Illegal instruction";

        case SIGSEGV:
            return "SIGSEGV: Segmentation violation";

        case SIGFPE:
            //const int exception = fetestexcept(FE_ALL_EXCEPT);
            //if(exception & FE_DIVBYZERO) puts("FE_DIVBYZERO");
            //if(exception & FE_INEXACT)   puts("FE_INEXACT");
            //if(exception & FE_INVALID)   puts("FE_INVALID");
            //if(exception & FE_OVERFLOW)  puts("FE_OVERFLOW");
            //if(exception & FE_UNDERFLOW) puts("FE_UNDERFLOW");
            return "SIGFPE: Floating point exception";

        default:
            return NULL;
    }
}

void dummySignalHandler( int signal )
{
    dummyEnvironment* environment = dummyGetCurrentEnvironment();
    if(environment)
    {
        // If dummySetAbortInformation was not called before:
        if(environment->errorCode == DUMMY_PROTECTED_CALL_SUCEEDED)
        {
            dummySetAbortInformation(
                DUMMY_PROTECTED_CALL_GENERIC_ERROR,
                dummySignalToAbortReason(signal));
        }

        longjmp(environment->jumpBuffer, 1);
    }
}

int dummyProtectedCall( dummyProtectableFunction fn, const char** abortReason )
{
    dummyEnvironment* environment = dummyPushEnvironment();
    const int jumpResult = setjmp(environment->jumpBuffer);
    if(jumpResult == 0)
    {
        fn();
    }
    else
    {
        // A signal has been catched: reset signals
        dummySetSignals(dummySignalHandler);
    }
    dummyPopEnvironment();

    if(abortReason)
    {
        if(environment->abortReason[0] == '\0')
            *abortReason = NULL;
        else
            *abortReason = environment->abortReason;
    }
    return environment->errorCode;
}

void dummySetAbortInformation( int errorCode, const char* reason )
{
    dummyEnvironment* environment = dummyGetCurrentEnvironment();
    assert(environment);

    environment->errorCode = errorCode;
    if(reason)
        strncpy(environment->abortReason, reason, DUMMY_MAX_PROTECTED_CALL_REASON_LENGTH);
}

void dummyAbortProtectedCall( int errorCode, const char* reason )
{
    dummySetAbortInformation(errorCode, reason);
    raise(SIGABRT);
}
