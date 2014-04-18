#ifndef __LAMER_XML_REPORTER_H__
#define __LAMER_XML_REPORTER_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports in a JUnit compatible XML format.
 */
const lamerReporter* lamerGetXMLReporter();

#ifdef __cplusplus
}
#endif

#endif
