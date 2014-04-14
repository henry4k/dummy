#include <stdio.h>
#include "core.h"
#include "require.h"

void testRequire_( const char* expression, int result )
{
    if(!result)
    {
        char reason[256];
        sprintf(reason, "%s failed", expression);
        testFailCase(reason);
    }
}
