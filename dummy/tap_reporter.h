#ifndef __DUMMY_TAP_REPORTER_H__
#define __DUMMY_TAP_REPORTER_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports using the 'test anything protocol' (TAP).
 *
 * Example:
 * @code
 * dummyGetTAPReporter(stdout)
 * @endcode
 *
 * @param file
 * The report is written to `file`.
 */
const dummyReporter* dummyGetTAPReporter();

#ifdef __cplusplus
}
#endif

#endif
