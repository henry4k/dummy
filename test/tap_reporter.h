#ifndef __TEST_TAP_REPORTER_H__
#define __TEST_TAP_REPORTER_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports using the 'test anything protocol' (TAP).
 */
const testReporter* testGetTAPReporter();

#ifdef __cplusplus
}
#endif

#endif
