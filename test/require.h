#ifndef __TEST_REQUIRE_H__
#define __TEST_REQUIRE_H__

#define testRequire( E ) testRequire_(#E, (E))

void testRequire_( const char* expression, int result );

#endif
