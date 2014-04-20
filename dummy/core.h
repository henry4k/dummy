#ifndef __DUMMY_CORE_H__
#define __DUMMY_CORE_H__

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
    DUMMY_UNKNOWN_TEST_COUNT = -1
};

typedef enum
{
    DUMMY_INITIALIZING,
    DUMMY_RUNNING,
    DUMMY_COMPLETED
} dummyStatus;

typedef enum
{
    DUMMY_TEST_UNDEFINED,
    DUMMY_TEST_STARTING,
    DUMMY_TEST_RUNNING,
    DUMMY_TEST_COMPLETED
} dummyTestStatus;

typedef enum
{
    DUMMY_TEST_PASSED,
    DUMMY_TEST_FAILED,
    DUMMY_TEST_SKIPPED
} dummyTestResult;

typedef enum
{
    DUMMY_FAIL_TEST,
    DUMMY_SKIP_TEST
} dummyTestAbortType;

typedef struct
{
    void* context;

    void (*began)( void* context );
    void (*completed)( void* context );

    void (*beganTest)( void* context );
    void (*completedTest)( void* context );

    void (*log)( void* context, const char* message );
} dummyReporter;

typedef void (*dummyCleanupFunction)( void* data );

/**
 * Function that is called for a test.
 */
typedef void (*dummyTestFunction)();


/**
 * Initializes the test context.
 *
 * This needs to be called before any calls to other test functions.
 * The initial context status is `DUMMY_INITIALIZING`.
 *
 * @param reporter
 * Reporter which will be used by the created context.
 */
void dummyInit( const dummyReporter* reporter );

/**
 * Runs all added tests and destroys the current context.
 *
 * @return
 * Number of test tests, that failed.
 */
int dummyRunTests();

/**
 * Adds a test to the current context.
 *
 * @param name
 * May be used by the reporter to describe the test.
 *
 * @param fn
 * Is called when the test is being run.
 * See #dummyRunTests
 *
 * @return
 * Id of the test.
 */
int dummyAddTest( const char* name, dummyTestFunction fn );

/**
 * Adds a cleanup function to the current test.
 * The function is called regardless whether the test succeeds or fails.
 *
 * @param data
 * Data pointer that is passed to the cleanup function.
 */
void dummyAddCleanup( dummyCleanupFunction fn, void* data );

/**
 * Status of the current context.
 */
dummyStatus dummyGetStatus();

/**
 * Count of tests added to the current context.
 */
int dummyGetTestCount();

/**
 * Name of the active test in the current context.
 */
const char* dummyGetTestName();

/**
 * Id of the active test in the current context.
 */
int dummyGetTestNumber();

/**
 * Status of the active test in the current context.
 */
dummyTestStatus dummyGetTestStatus();

/**
 * TODO
 */
dummyTestResult dummyGetTestResult();

/**
 * Message describing why the current test aborted
 * or `NULL` if the test hasn't failed (yet).
 */
const char* dummyGetTestAbortReason();

int dummyTestIsMarkedAsTodo();

const char* dummyGetTestTodoReason();

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
void dummyAbortTest( dummyTestAbortType type, const char* reason, ... );

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
void dummyMarkTestAsTodo( const char* reason, ... );


/**
 * Reports a diagnostic message.
 */
void dummyLog( const char* message, ... );


#ifdef __cplusplus
}
#endif

#endif
