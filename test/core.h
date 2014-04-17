#ifndef __TEST_CORE_H__
#define __TEST_CORE_H__

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
    TEST_UNKNOWN_CASE_COUNT = -1
};

typedef enum
{
    TEST_INITIALIZING,
    TEST_RUNNING,
    TEST_COMPLETED
} testStatus;

typedef enum
{
    TEST_CASE_UNDEFINED,
    TEST_CASE_STARTING,
    TEST_CASE_RUNNING,
    TEST_CASE_COMPLETED
} testCaseStatus;

typedef enum
{
    TEST_CASE_PASSED,
    TEST_CASE_FAILED,
    TEST_CASE_SKIPPED
} testCaseResult;

typedef enum
{
    TEST_FAIL_CASE,
    TEST_SKIP_CASE
} testAbortType;

typedef struct
{
    void* context;

    void (*began)( void* context );
    void (*completed)( void* context );

    void (*beganCase)( void* context );
    void (*completedCase)( void* context );

    void (*diag)( void* context, const char* message );
} testReporter;


/**
 * Initializes the test context.
 *
 * This needs to be called before any calls to other test functions.
 * The initial context status is `TEST_INITIALIZING`.
 *
 * @param reporter
 * Reporter which will be used by the created context.
 */
void testInit( const testReporter* reporter );

/**
 * Runs all added test cases and destroys the current context.
 *
 * @return
 * Number of tests, that failed.
 */
int testRun();

/**
 * Function that is called for a test case.
 */
typedef void (*testCaseFunction)();

/**
 * Adds a test case to the current context.
 *
 * @param name
 * May be used by the reporter to describe the test case.
 *
 * @param fn
 * Is called when the test case is being run.
 * See #testRun
 *
 * @return
 * Id of the test case.
 */
int testAddCase( const char* name, testCaseFunction fn );

/**
 * Status of the current test context.
 */
testStatus testGetStatus();

/**
 * Count of test cases added to the current test context.
 */
int testGetCaseCount();

/**
 * Name of the active test case in the current context.
 */
const char* testGetCaseName();

/**
 * Id of the active test case in the current context.
 */
int testGetCaseNumber();

/**
 * Status of the active case in the current context.
 */
testCaseStatus testGetCaseStatus();

/**
 *
 */
testCaseResult testGetCaseResult();

/**
 * Message describing why the current test case aborted
 * or `NULL` if the test case hasn't failed (yet).
 */
const char* testGetCaseAbortReason();

int testCaseIsMarkedAsTodo();

const char* testGetCaseTodoReason();

/**
 * Aborts current test case with the given reason.
 *
 * @param reason
 * Explains why the test case was aborted - this information is available
 * to the test reporter. May be `NULL` to signal an unknown reason.
 *
 * @return
 * Doesn't return.
 */
void testAbortCase( testAbortType type, const char* reason, ... );

/**
 * Marks current test case as TODO.
 *
 * @param reason
 * Explains why the test case has been marked as TODO -
 * this information is available to the test reporter.
 * May be `NULL` to signal an unknown reason.
 *
 * @note
 * Calling this function more than once in a test case,
 * may overwrite the previous TDO reason.
 */
void testMarkCaseAsTodo( const char* reason, ... );


#ifdef __cplusplus
}
#endif

#endif
