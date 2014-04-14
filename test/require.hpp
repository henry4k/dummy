#ifndef __TEST_REQUIRE_HPP__
#define __TEST_REQUIRE_HPP__

namespace test
{

// bool, long, double, string, void*

class Comparison
{
public:
    Comparison( const char* expression );

protected:
    const char* m_Comparison;
};

class BoolComparison : public Comparison
{
public:
    BoolComparison( const char* expression, bool value );
    BoolComparison& is( bool value );
    BoolComparison& equals( bool value );

private:
    bool m_Value;
};


#define RequireThat( E ) RequireThat_(#E, E)
BoolComparison RequireThat_( const char* expression, bool value );


// ---- bool implementation ----

BoolComparison::BoolComparison( const char* expression, bool value ) :
    Comparison(expression),
    m_Value(value)
{
}

BoolComparison& BoolComparison::is( bool value )
{
    return equals(value);
}

BoolComparison& BoolComparison::equals( bool value )
{
    const char* expected = (value == true) ? "true" : "false";
    const char* got = (m_Value == true) ? "true" : "false";

    if(m_Value != value)
        testFail("Expected %s to be %s, but it was %s.",
            m_Comparison,
            expected,
            got);
    return *this;
}

BoolComparison RequireThat_( const char* expression, bool value )
{
    return BoolComparison(expression, value);
}


// ---- int implementation ----

IntComparison::IntComparison( const char* expression, long value ) :
    Comparison(expression),
    m_Value(value)
{
}

BoolComparison& BoolComparison::isTrue()
{
    return equals(true);
}

BoolComparison& BoolComparison::isFalse()
{
    return equals(false);
}

BoolComparison& BoolComparison::equals( bool value )
{
    const char* expected = (value == true) ? "true" : "false";
    const char* got = (m_Value == true) ? "true" : "false";

    if(m_Value != value)
        testFail("Expected %s to be %s, but it was %s.",
            m_Comparison,
            expected,
            got);
    return *this;
}

BoolComparison RequireThat_( const char* expression, bool value )
{
    return BoolComparison(expression, value);
}


}

#endif
