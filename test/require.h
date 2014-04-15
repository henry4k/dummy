#ifndef __TEST_REQUIRE_H__
#define __TEST_REQUIRE_H__

#include <stdio.h>
#include "core.h"

#define testRequire( E ) testRequire_(#E, (E))

static inline void testRequire_( const char* expression, int result )
{
    if(!result)
    {
        char reason[256];
        sprintf(reason, "%s failed", expression);
        testFailCase(reason);
    }
}

#endif
