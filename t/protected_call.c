#include <stdio.h>
#include <signal.h>

#include "../dummy/protected_call.h"


void pcall_2()
{
    raise(SIGABRT);
}

void pcall_1()
{
    const char* abortReason = "bbb";
    const int errorCode = dummyProtectedCall(pcall_2, &abortReason);
    if(errorCode != DUMMY_PROTECTED_CALL_SUCEEDED)
        dummyAbortProtectedCall(errorCode, abortReason);
}

int main()
{
    const char* abortReason = "aaa";
    const int errorCode = dummyProtectedCall(pcall_1, &abortReason);
    printf("error code = %d, abort reason = '%s'\n",
        errorCode,
        abortReason);
    return 0;
}
