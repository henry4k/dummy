#ifndef __DUMMY_PROTECTED_CALL_H__
#define __DUMMY_PROTECTED_CALL_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*dummyProtectableFunction)();

typedef void (*dummyCleanupFunction)( void* data );

enum
{
    DUMMY_PROTECTED_CALL_SUCEEDED = 0,
    DUMMY_PROTECTED_CALL_GENERIC_ERROR = 1
};

/**
 * Runs the given function and catches errors.
 *
 * Protected calls can be nested.
 *
 * @param abortReason
 * If the protected call is aborted with a reason,
 * the string pointer is passed here.
 *
 * @return
 * Code that classifies the error.
 * If the function ras successfully it returns DUMMY_PROTECTED_CALL_SUCEEDED.
 * Custom error codes may be passed using dummyAbortProtectedCall().
 */
int dummyProtectedCall( dummyProtectableFunction fn, const char** abortReason );

/**
 * Adds a cleanup function to the current protected call.
 *
 * @param data
 * Data pointer that is passed to the cleanup function.
 */
void dummyAddCleanup( dummyCleanupFunction fn, void* data );

/**
 * Aborts the current protected call.
 *
 * @param reason
 * May be `NULL`.
 *
 * @return
 * Doesn't return.
 */
void dummyAbortProtectedCall( int errorCode, const char* reason );

#ifdef __cplusplus
}
#endif

#endif
