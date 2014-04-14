#ifndef __TEST_XML_REPORTER_H__
#define __TEST_XML_REPORTER_H__

#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Reports in a JUnit compatible XML format.
 */
const testReporter* testGetXMLReporter();

#ifdef __cplusplus
}
#endif

#endif
