#ifndef __TEST_BDD_HPP__
#define __TEST_BDD_HPP__

#include <string>
#include "core.h"

namespace test
{

class Describe
{
public:
    Describe( const char* subject );
    Describe& it( const char* behaviour, testCaseFunction fn );

private:
    std::string m_Subject;
};


// ---- implementation ----

Describe::Describe( const char* subject ) :
    m_Subject(subject)
{
}

Describe& Describe::it( const char* behaviour, testCaseFunction fn )
{
    testAddCase((m_Subject+" "+behaviour).c_str(), fn);
    return *this;
}

}

#endif
