#ifndef __DUMMY_CORE_H__
#define __DUMMY_CORE_H__

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
    #define _DUMMY_IMPORT __declspec(dllimport)
    #define _DUMMY_EXPORT __declspec(dllexport)
#else
    #if __GNUC__ >= 4
        #define _DUMMY_IMPORT __attribute__ ((visibility ("default")))
        #define _DUMMY_EXPORT __attribute__ ((visibility ("default")))
    #else
        #define _DUMMY_IMPORT
        #define _DUMMY_EXPORT
    #endif
#endif

// Now we use the generic helper definitions above to define DUMMY_API.
// DUMMY_API is used for the public API symbols. It either DLL imports or
// DLL exports (or does nothing for static build)

#ifdef DUMMY_SHARED // defined if DUMMY is compiled as a DLL
    #ifdef DUMMY_BUILDING_SHARED // defined if we are building the DUMMY DLL (instead of using it)
        #define DUMMY_API _DUMMY_EXPORT
    #else
        #define DUMMY_API _DUMMY_IMPORT
    #endif // DUMMY_BUILDING_SHARED
#else // DUMMY_SHARED is not defined: this means DUMMY is a static lib.
    #define DUMMY_API
#endif


#ifdef __cplusplus
extern "C"
{
#endif


enum
{
    DUMMY_UNKNOWN_TEST_COUNT = -1,
    DUMMY_MAX_SANDBOX_DEPTH = 32
};

/**
 * Standard error codes, that can be returned by #dummySandbox functions.
 */
enum
{
    DUMMY_SANDBOX_SUCCEEDED = 0,
    DUMMY_SANDBOX_GENERIC_ERROR = 1
};

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


/**
 * Can be called in a sandbox.
 */
typedef void (*dummySandboxableFunction)();

/**
 * Runs the given function and catches errors.
 *
 * @param abortReason
 * If the protected call is aborted with a reason,
 * the string pointer is passed here.
 *
 * @return
 * Code that classifies the error.
 * If the function ran successfully it returns #DUMMY_SANDBOX_SUCCEEDED.
 * Custom error codes may be passed using #dummyAbortSandbox.
 */
typedef int (*dummySandbox)( dummySandboxableFunction fn, const char** abortReason );

/**
 * Aborts the current sandbox call.
 *
 * @param reason
 * May be `NULL`.
 *
 * @return
 * Doesn't return.
 */
typedef void (*dummyAbortHandler)( void* context, int errorCode, const char* reason );

typedef void (*dummyCleanupFunction)( void* data );

typedef struct
{
    void* context;

    void (*began)( void* context );
    void (*completed)( void* context );

    void (*beganTest)( void* context );
    void (*completedTest)( void* context );

    void (*log)( void* context, const char* message );
} dummyReporter;


/**
 * Initializes the test context.
 *
 * This needs to be called before any calls to other test functions.
 * The initial context status is #DUMMY_INITIALIZING.
 *
 * @param reporter
 * Reporter which will be used by the created context.
 */
DUMMY_API void dummyInit( const dummyReporter* reporter );

/**
 * Runs all added tests and destroys the current context.
 *
 * @return
 * Number of tests, that failed.
 */
DUMMY_API int dummyRunTests();

/**
 * Adds a test to the current context.
 *
 * @param name
 * May be used by the reporter to describe the test.
 *
 * @param sandbox
 * Sandbox that is being used to run the test.
 *
 * @param fn
 * Is called when the test is being run.
 *
 * @see dummyRunTests
 */
DUMMY_API void dummyAddTest( const char* name, dummySandbox sandbox, dummySandboxableFunction fn );

/**
 * Count of tests added to the current context.
 */
DUMMY_API int dummyGetTestCount();

/**
 * Name of the active test in the current context.
 */
DUMMY_API const char* dummyGetTestName();

/**
 * Id of the active test in the current context.
 */
DUMMY_API int dummyGetTestNumber();

/**
 * TODO
 */
DUMMY_API dummyTestResult dummyGetTestResult();

/**
 * Message describing why the current test aborted
 * or `NULL` if the test hasn't failed (yet).
 */
DUMMY_API const char* dummyGetTestAbortReason();

/**
 * Adds a cleanup function to the current test.
 *
 * @param data
 * Data pointer that is passed to the cleanup function.
 */
DUMMY_API void dummyAddCleanup( dummyCleanupFunction fn, void* data );

DUMMY_API int dummyTestIsMarkedAsTodo();

/**
 * If set, it returns the reason why the current test has been marked as TODO.
 * May be `NULL` if no reason has been given.
 */
DUMMY_API const char* dummyGetTestTodoReason();

/**
 * Aborts current test with the given reason.
 *
 * @param reason
 * Explains why the test was aborted -
 * this information is available to the reporter.
 *
 * @return
 * Doesn't return.
 */
DUMMY_API void dummyAbortTest( dummyTestAbortType type, const char* reason, ... ); // TODO: Can be replaced with dummyAbortSandbox

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
DUMMY_API void dummyMarkTestAsTodo( const char* reason, ... );

/**
 * Reports a diagnostic message.
 */
DUMMY_API void dummyLog( const char* message, ... );

/**
 * Replaces the current abort implementation.
 */
DUMMY_API void dummyPushAbortHandler( dummyAbortHandler implementation, void* context );

/**
 * Resets the current abort implementation to the last one or disables it.
 */
DUMMY_API void dummyPopAbortHandler();

/**
 * Aborts the current sandbox using the current abort implementation.
 *
 * @note
 * Trying to abort without an abort implementation will cause an fatal error.
 */
DUMMY_API void dummyAbortSandbox( int errorCode, const char* reason );


#ifdef __cplusplus
}
#endif

#endif
