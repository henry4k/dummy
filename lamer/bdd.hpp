#ifndef __LAMER_BDD_HPP__
#define __LAMER_BDD_HPP__

#include <string>
#include "core.h"

namespace lamer
{

class Describe
{
public:
    Describe( const char* subject );
    Describe& it( const char* behaviour, lamerTestFunction fn );

private:
    std::string m_Subject;
};


// ---- implementation ----

Describe::Describe( const char* subject ) :
    m_Subject(subject)
{
}

Describe& Describe::it( const char* behaviour, lamerTestFunction fn )
{
    lamerAddTest((m_Subject+" "+behaviour).c_str(), fn);
    return *this;
}

}

#endif
