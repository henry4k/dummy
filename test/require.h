#ifndef __TEST_REQUIRE_H__
#define __TEST_REQUIRE_H__

#include "core.h"

#define testRequire( E ) testRequire_(#E, (E))

static inline void testRequire_( const char* expression, int result )
{
    if(!result)
        testAbortCase(TEST_FAIL_CASE, "%s failed", expression);
}

#endif
