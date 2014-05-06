#include <stddef.h> // NULL
#include <stdlib.h> // malloc, free

#include "inline.h"


typedef struct InlineTest_
{
    const char* name;
    dummyTestFunction fn;
    InlineTest_* next;
} InlineTest;

static InlineTest* FirstInlineTest = NULL;
static InlineTest* LastInlineTest = NULL;

void dummyAddInlineTests()
{
    InlineTest* current = FirstInlineTest;
    while(current)
    {
        dummyAddTest(
            current->name,
            current->fn);

        InlineTest* previous = current;
        current = current->next;
        free(previous);
    }
    FirstInlineTest = NULL;
    LastInlineTest = NULL;
}

int dummyAddInlineTest_( const char* name, dummyTestFunction fn )
{
    InlineTest* test = (InlineTest*)malloc(sizeof(InlineTest));
    test->name = name;
    test->fn = fn;

    if(!FirstInlineTest)
        FirstInlineTest = test;

    // Append to end
    if(LastInlineTest)
        LastInlineTest->next = test;
    LastInlineTest = test;
    test->next = NULL;

    return 1;
}
