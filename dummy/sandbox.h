#ifndef __DUMMY_SANDBOX_H__
#define __DUMMY_SANDBOX_H__

#ifdef __cplusplus
extern "C"
{
#endif


enum
{
    DUMMY_SANDBOX_SUCCEEDED = 0,
    DUMMY_SANDBOX_GENERIC_ERROR = 1
};


/**
 * Can be called in a sandbox.
 */
void (*dummySandboxableFunction)();

/**
 * Runs the given function and catches errors.
 *
 * @param abortReason
 * If the protected call is aborted with a reason,
 * the string pointer is passed here.
 *
 * @return
 * Code that classifies the error.
 * If the function ras successfully it returns DUMMY_SANDBOX_SUCCEEDED.
 * Custom error codes may be passed using dummyAbortSandbox().
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
void (*dummyAbortImplementation)( void* context, int errorCode, const char* reason );


/**
 * Replaces the current abort implementation.
 */
void dummyPushAbortImplementation( dummyAbortImplementation implementation, void* context );

/**
 * Resets the current abort implementation to the last one or disables it.
 */
void dummyPopAbortImplementation();

/**
 * Aborts the current sandbox using the current abort implementation.
 *
 * @note
 * Trying to abort without an abort implementation will cause an fatal error.
 */
void dummyAbortSandbox( int errorCode, const char* reason );


#ifdef __cplusplus
}
#endif

#endif
