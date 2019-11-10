/**
 * This file just contains tools for signal and exception runner.
 * You don't want to use it directly.
 */
#ifndef __DUMMY_SIGNAL_UTILS_H__
#define __DUMMY_SIGNAL_UTILS_H__

#include <signal.h>
#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*dummySignalHandler)(int);

DUMMY_API void dummyPushSignalHandler( dummySignalHandler fn );
DUMMY_API void dummyPopSignalHandler();

DUMMY_API const char* dummySignalToAbortReason( int signal );

#ifdef __cplusplus
}
#endif

#endif
