#ifndef __DUMMY_REQUIRE_HPP__
#define __DUMMY_REQUIRE_HPP__

#include "core.h"
#include "require.h"
#include <string.h> // strcmp, strlen
#include <sstream> // std::ostringstream

template<typename T>
bool _dummyEquals( T a, T b )
{
    return a == b;
}

template<char const *>
bool _dummyEquals( char const * a, char const * b )
{
    return strcmp(a, b) == 0;
}

#define dummyRequireEquals( expected, actual ) \
        dummyRequireEquals_((expected), (actual), \
                            #expected, #actual, \
                            __FILE__, __LINE__)
template<typename T>
void dummyRequireEquals_( T expected, T actual,
                          const char* expectedRepresentation,
                          const char* actualRepresentation,
                          const char* file, int line )
{
    if(!_dummyEquals(expected, actual))
    {
        using namespace std;
        ostringstream s;
        s << "In " << file << ":" << line << ":" << endl;
        s << "    " << actualRepresentation << " == " << expectedRepresentation << " failed" << endl;
        s << "was ";
        s.flags(ios::right);
        s.width(strlen(actualRepresentation));
        s << actual << "    ";
        s.flags(ios::left);
        s.width(strlen(expectedRepresentation));
        s << expected;
        dummyAbortTest(DUMMY_FAIL_TEST, s.str().c_str());
    }
}


#endif
