#ifndef __DUMMY_EXCEPTION_SANDBOX_H__
#define __DUMMY_EXCEPTION_SANDBOX_H__

#include "core.h"
#include "config.h"

#if !defined(DUMMY_CXX_MODE)
#warning "DUMMY_CXX_MODE is not defined, error descriptions may not be accurate."
#endif

#ifdef __cplusplus
extern "C"
{
#endif

const dummySandbox* dummyGetExceptionSandbox();

#ifdef __cplusplus
}
#endif

#endif
