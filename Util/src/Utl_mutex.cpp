#include "../include/Comm.h"
#include "../include/Utl_mutex.h"
#include "../include/vpl_error.h"
#include "../include/Utl_time.h"

static void privTimeoutToTimespec(const struct timespec* in, UTLTime_t timeout, struct timespec* out)
{
    out->tv_sec = in->tv_sec + (time_t)UTLTime_ToSec(timeout);
    out->tv_nsec = in->tv_nsec + (time_t)UTLTime_ToNanosec(timeout%UTLTIME_MICROSEC_PER_SEC);
    out->tv_sec += out->tv_nsec/UTLTIME_NANOSEC_PER_SEC;
    out->tv_nsec %= UTLTIME_NANOSEC_PER_SEC;

    // If desired wait time is past the end of the epoch, wait until
    // the end of the epoch. This should only happen if the wait was
    // arbitrarily large, not because this code is in use near the end
    // of the epoch.
    if(out->tv_sec < in->tv_sec ||
       (out->tv_sec == in->tv_sec &&
        out->tv_nsec < in->tv_nsec)) {
        out->tv_sec = INT32_MAX;
        out->tv_nsec = 999999999;
    }
}

int UTLMutex_Init(UTLMutex_t* mutex)
{
    int rv = VPL_OK;
    int rc;
    //    static const int mutex_type = PTHREAD_MUTEX_TIMED_NP;
    if (mutex == NULL) {
        return VPL_ERR_INVALID;
    }

    mutex->lockTid = 0;
    mutex->lockCount = 0;
    rc = pthread_mutex_init((pthread_mutex_t *)(mutex->pthMutex), (pthread_mutexattr_t *)NULL);
    if (rc != 0) {
        rv = VPLError_XlatErrno(rc);
    } else {
        UTL_SET_INITIALIZED(mutex);
    }
    return rv;
}

int UTLMutex_Lock(UTLMutex_t* mutex){
    int rc;
    int rv = VPL_OK;
    pthread_t self = pthread_self();
    if (mutex == NULL) {
        return VPL_ERR_INVALID;
    }
    if (!UTL_IS_INITIALIZED(mutex)) {
        return VPL_ERR_INVALID;
    }
    if(UTLMutex_LockedSelf(mutex)) {
        mutex->lockCount++;
    } else {
        rc = pthread_mutex_lock((pthread_mutex_t *)(mutex->pthMutex));
        if(rc == 0) {
            rv = VPL_OK;
            // Note: this assignment of lockTid must be atomic.
            mutex->lockTid = self;
            mutex->lockCount = 1;
        } else {
            rv = VPLError_XlatErrno(rc);
        }
    }
    return rv;
}

int UTLMutex_Unlock(UTLMutex_t* mutex)
{
    int rv = VPL_OK;
    int rc;
    if (mutex == NULL) {
        return VPL_ERR_INVALID;
    }
    if (!UTL_IS_INITIALIZED(mutex)) {
        return VPL_ERR_INVALID;
    }
    if(!UTLMutex_LockedSelf(mutex)) {
        rv = VPL_ERR_PERM;
    } else if(--mutex->lockCount == 0) {
        // Note: this assignment of lockTid must be atomic.
        mutex->lockTid = 0;
        rc = pthread_mutex_unlock((pthread_mutex_t *)(mutex->pthMutex));
        if(rc == 0) {
            rv = VPL_OK;
        } else {
            rv = VPLError_XlatErrno(rc);
        }
    }
    return rv;
}


int UTLMutex_LockedSelf(const UTLMutex_t* mutex)
{
    if (mutex == NULL) {
        return VPL_ERR_INVALID;
    }
    // Note: this read of lockTid must be atomic.
    return ((pthread_t)((mutex)->lockTid) == pthread_self())? 1 : 0;
}

int UTLMutex_Destroy(UTLMutex_t* mutex)
{
    int rv;
    int rc;
    if (mutex == NULL) {
        return VPL_ERR_INVALID;
    }
    if (!UTL_IS_INITIALIZED(mutex)) {
        return VPL_ERR_INVALID;
    }
    rc = pthread_mutex_destroy((pthread_mutex_t *)(mutex->pthMutex));
    if (rc == 0) {
        UTL_SET_UNINITIALIZED(mutex);
        rv = VPL_OK;
    } else {
        rv = VPLError_XlatErrno(rc);
    }
    return rv;
}

//////////////////////////////////////////////////////////////////
int UTLSlimRWLock_UnlockRead(UTLSlimRWLock_t* rwlock)
{
    int rv = VPL_OK;
    if(rwlock == NULL) {
        rv = VPL_ERR_INVALID;
    } else {
        int rc = pthread_rwlock_unlock((pthread_rwlock_t*)&(rwlock->rwlock));
        if(rc != 0) {
            rv = VPLError_XlatErrno(rc);
        }
    }
    return rv; 
}

int UTLSlimRWLock_LockWrite(UTLSlimRWLock_t* rwlock){
    int rv = VPL_OK;
    if(rwlock == NULL) {
        rv = VPL_ERR_INVALID;
    } else {
        int rc = pthread_rwlock_wrlock((pthread_rwlock_t*)&(rwlock->rwlock));
        if(rc != 0) {
            rv = VPLError_XlatErrno(rc);
        }
    }
    return rv;
}

int UTLSlimRWLock_UnlockWrite(UTLSlimRWLock_t* rwlock)
{
    return UTLSlimRWLock_UnlockRead(rwlock); 
}

int UTLSlimRWLock_LockRead(UTLSlimRWLock_t* rwlock)
{
    int rv = VPL_OK;
    if(rwlock == NULL) {
        rv = VPL_ERR_INVALID;
    } else {
        int rc = pthread_rwlock_rdlock((pthread_rwlock_t*)&(rwlock->rwlock));
        if(rc != 0) {
            rv = VPLError_XlatErrno(rc);
        }
    }
    return rv; 
}

int UTLSlimRWLock_Init(UTLSlimRWLock_t* rwlock)
{
    int rv = VPL_OK;
    if(rwlock == NULL) {
        rv = VPL_ERR_INVALID;
    } else {
        // TODO: wish we could do this at compile time
        UTLIMPL_ASSERT(ARRAY_SIZE_IN_BYTES(rwlock->rwlock) >= sizeof(pthread_rwlock_t));
        int rc = pthread_rwlock_init((pthread_rwlock_t*)&(rwlock->rwlock), NULL);
        if(rc != 0) {
            rv = VPLError_XlatErrno(rc);
        }
    }
    return rv; 
}

//////////////////////////////////////////////////////////////////

int UTLCond_Init(UTLCond_t* cond)
{
    int rv;

    if (cond == NULL) {
        return VPL_ERR_INVALID;
    }

    if (UTL_IS_INITIALIZED(cond)) {
        return VPL_ERR_IS_INIT;
    }

    // TODO: bug 521: init with pthread_condattr_setclock as CLOCK_MONOTONIC.
    rv = pthread_cond_init((pthread_cond_t *)(cond->cond), NULL);
    if (rv == 0) {
        UTL_SET_INITIALIZED(cond);
        return VPL_OK;
    }
    else {
        return VPLError_XlatErrno(rv);
    }
}

int UTLCond_Signal(UTLCond_t* cond)
{
    int rv;

    if (cond == NULL) {
        return VPL_ERR_INVALID;
    }
    if (!UTL_IS_INITIALIZED(cond)) {
        return VPL_ERR_INVALID;
    }

    rv = pthread_cond_signal((pthread_cond_t *)(cond->cond));
    if (rv == 0) {
        return VPL_OK;
    } else {
        return VPLError_XlatErrno(rv);
    }
}

int UTLCond_Broadcast(UTLCond_t* cond)
{
    int rv;

    if (cond == NULL) {
        return VPL_ERR_INVALID;
    }
    if (!UTL_IS_INITIALIZED(cond)) {
        return VPL_ERR_INVALID;
    }

    rv = pthread_cond_broadcast((pthread_cond_t *)(cond->cond));
    if (rv == 0) {
        return VPL_OK;
    } else {
        return VPLError_XlatErrno(rv);
    }
}

int UTLCond_TimedWait(UTLCond_t* cond, UTLMutex_t* mutex, UTLTime_t time)
{
    int count;
    int rv = VPL_OK;

    if (cond == NULL) {
        return VPL_ERR_INVALID;
    }
    if (!UTL_IS_INITIALIZED(cond)) {
        return VPL_ERR_INVALID;
    }
    if (mutex == NULL) {
        return VPL_ERR_INVALID;
    }
    if (!UTL_IS_INITIALIZED(mutex)) {
        return VPL_ERR_INVALID;
    }

    count = mutex->lockCount;
    mutex->lockCount = 0;
    // Note: this assignment of lockTid must be atomic.
    mutex->lockTid = 0;

    if (time == UTL_TIMEOUT_NONE) {
        int rc = pthread_cond_wait((pthread_cond_t *)(cond->cond),
                (pthread_mutex_t *)(mutex->pthMutex));
        if (rc == 0) {
            rv = VPL_OK;
        }
        else {
            rv = VPLError_XlatErrno(rc);
        }
    }
    else {
        struct timespec now;
        // TODO: bug 521: use CLOCK_MONOTONIC
        if(clock_gettime(CLOCK_REALTIME, &now) == 0) {
            int rc;
            struct timespec ts;
            privTimeoutToTimespec(&now, time, &ts);
            rc = pthread_cond_timedwait((pthread_cond_t *)(cond->cond),
                    (pthread_mutex_t *)(mutex->pthMutex), &ts);
            // Upon return, the mutex should be locked again (even in failure cases).
            if (rc == 0) {
                rv = VPL_OK;
            }
            else {
                rv = VPLError_XlatErrno(rc);
            }
        }
        else {
            rv = VPL_ERR_FAIL;
        }
    }

    mutex->lockCount = count;
    // Note: this assignment of lockTid must be atomic.
    mutex->lockTid = pthread_self();
    return rv;
}

int UTLCond_Destroy(UTLCond_t* cond)
{
    int rv = VPL_OK;
    int rc;

    if (cond == NULL) {
        return VPL_ERR_INVALID;
    }
    if (!UTL_IS_INITIALIZED(cond)) {
        return VPL_ERR_INVALID;
    }

    rc = pthread_cond_destroy((pthread_cond_t *)(cond->cond));
    if (rc != 0) {
        rv = VPLError_XlatErrno(rc);
    }

    UTL_SET_UNINITIALIZED(cond);
    return rv;
}  

