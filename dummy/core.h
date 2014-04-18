#ifndef __LAMER_CORE_H__
#define __LAMER_CORE_H__

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
    LAMER_UNKNOWN_TEST_COUNT = -1
};

typedef enum
{
    LAMER_INITIALIZING,
    LAMER_RUNNING,
    LAMER_COMPLETED
} lamerStatus;

typedef enum
{
    LAMER_TEST_UNDEFINED,
    LAMER_TEST_STARTING,
    LAMER_TEST_RUNNING,
    LAMER_TEST_COMPLETED
} lamerTestStatus;

typedef enum
{
    LAMER_TEST_PASSED,
    LAMER_TEST_FAILED,
    LAMER_TEST_SKIPPED
} lamerTestResult;

typedef enum
{
    LAMER_FAIL_TEST,
    LAMER_SKIP_TEST
} lamerTestAbortType;

typedef struct
{
    void* context;

    void (*began)( void* context );
    void (*completed)( void* context );

    void (*beganTest)( void* context );
    void (*completedTest)( void* context );

    void (*diag)( void* context, const char* message );
} lamerReporter;


/**
 * Initializes the test context.
 *
 * This needs to be called before any calls to other test functions.
 * The initial context status is `LAMER_INITIALIZING`.
 *
 * @param reporter
 * Reporter which will be used by the created context.
 */
void lamerInit( const lamerReporter* reporter );

/**
 * Runs all added tests and destroys the current context.
 *
 * @return
 * Number of test tests, that failed.
 */
int lamerRunTests();

/**
 * Function that is called for a test.
 */
typedef void (*lamerTestFunction)();

/**
 * Adds a test to the current context.
 *
 * @param name
 * May be used by the reporter to describe the test.
 *
 * @param fn
 * Is called when the test is being run.
 * See #lamerRunTests
 *
 * @return
 * Id of the test.
 */
int lamerAddTest( const char* name, lamerTestFunction fn );

/**
 * Status of the current context.
 */
lamerStatus lamerGetStatus();

/**
 * Count of tests added to the current context.
 */
int lamerGetTestCount();

/**
 * Name of the active test in the current context.
 */
const char* lamerGetTestName();

/**
 * Id of the active test in the current context.
 */
int lamerGetTestNumber();

/**
 * Status of the active test in the current context.
 */
lamerTestStatus lamerGetTestStatus();

/**
 * TODO
 */
lamerTestResult lamerGetTestResult();

/**
 * Message describing why the current test aborted
 * or `NULL` if the test hasn't failed (yet).
 */
const char* lamerGetTestAbortReason();

int lamerTestIsMarkedAsTodo();

const char* lamerGetTestTodoReason();

/**
 * Aborts current test with the given reason.
 *
 * @param reason
 * Explains why the test was aborted - this information is available
 * to the reporter. May be `NULL` to signal an unknown reason.
 *
 * @return
 * Doesn't return.
 */
void lamerAbortTest( lamerTestAbortType type, const char* reason, ... );

/**
 * Marks current test as TODO.
 *
 * @param reason
 * Explains why the test has been marked as TODO -
 * this information is available to the reporter.
 * May be `NULL` to signal an unknown reason.
 *
 * @note
 * Calling this function more than once in a test,
 * may overwrite the previous TODO reason.
 */
void lamerMarkTestAsTodo( const char* reason, ... );


#ifdef __cplusplus
}
#endif

#endif
