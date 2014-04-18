#ifndef __LAMER_MAX_REPORTER_H__
#define __LAMER_MAX_REPORTER_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports as many information as possible using a custom format.
 *
 * @note
 * It is used for self lamering and not intended to be used externally,
 * since the format may change over time.
 */
const lamerReporter* lamerGetMaxReporter();

#ifdef __cplusplus
}
#endif

#endif
