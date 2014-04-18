#ifndef __LAMER_REQUIRE_H__
#define __LAMER_REQUIRE_H__

#include "core.h"

#define lamerRequire( E ) lamerRequire_(#E, (E))

static inline void lamerRequire_( const char* expression, int result )
{
    if(!result)
        lamerAbortTest(LAMER_FAIL_TEST, "%s failed", expression);
}

#endif
