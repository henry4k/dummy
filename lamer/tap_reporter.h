#ifndef __LAMER_TAP_REPORTER_H__
#define __LAMER_TAP_REPORTER_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports using the 'lamer anything protocol' (TAP).
 */
const lamerReporter* lamerGetTAPReporter();

#ifdef __cplusplus
}
#endif

#endif
