#include "../include/Utl_thread.h"
#include "../include/vpl_error.h"
#include "../include/Utl_Log.h"

static const UTLThread_attr_t utlThread__default_attrs = {
    UTLTHREAD_STACKSIZE_DEFAULT,
    // Documented VPL stack-size.
    false,
    UTLTHREAD_PRIO_INHERIT,
    // To be overwritten at runtime with the calling thread's priority.
    SCHED_OTHER,
    // Default nativeSchedPolicy.
    0,
    // Default nativeSchedPrio.
    NULL
    // Default stack address (unused)
};

static int UTLThread_PrioMapHelper(int vplPrio)
{
    int pthreadMinPrio = 1;
    int pthreadMaxPrio = UTLTHREAD_PTHREAD_PRIO_MAX;
    int vplMinPrio = UTL_PRIO_MIN;
    int vplMaxPrio = UTL_PRIO_MAX;

    if (vplMaxPrio == vplMinPrio) {
        // Prevent division by zero later.
        return pthreadMinPrio;
    }
    // Priority is linearly mapped from the UTL thread priority to POSIX thread priority.
    double relativePrio = (double)(vplPrio - vplMinPrio) / (double)(vplMaxPrio - vplMinPrio);
    int nativeSchedPrio = pthreadMinPrio + (int)(relativePrio * (pthreadMaxPrio - pthreadMinPrio));
    return nativeSchedPrio;
}

static int UTLThread_MapPrioToNativePrio(UTLThread__prio_mapping_t* nativePrio, int vplPrio)
{
    // Special case: default POSIX thread priority.
    if (vplPrio == UTL_PRIO_MIN) {
        nativePrio->policy = SCHED_OTHER;
        nativePrio->params.sched_priority = 0;
        return VPL_OK;
    }
    // Otherwise, policy is fixed to SCHED_RR.
    nativePrio->policy = SCHED_RR;
    if (vplPrio < UTL_PRIO_MIN || vplPrio > UTL_PRIO_MAX) {
        // Can't do anything with an invalid VPL thread priority.
        nativePrio->params.sched_priority = UTLTHREAD_PRIO_INHERIT;
        return VPL_ERR_INVALID;
    }
    nativePrio->params.sched_priority = UTLThread_PrioMapHelper(vplPrio);
    return VPL_OK;
}

static void UTLThread_AttrsGetPrio(UTLThread_attr_t* attrs)
{
    UTLThread__prio_mapping_t nativePrio;
    int vplPrio;
    int status;

    memset(&nativePrio, 0, sizeof(nativePrio));
    /// Get the VPL priority, normalizing a possible non-UTL priority.
    vplPrio = UTLThread_GetSchedPriority();
    if (vplPrio < UTL_PRIO_MIN || vplPrio > UTL_PRIO_MAX) {
        UTL_LOG_WARN("UTLThread internal error: impossible priority %d", vplPrio);
    }
    // Map that UTL priority back to a native (scheduler policy, params) pair.
    status = UTLThread_MapPrioToNativePrio(&nativePrio, vplPrio);
    UNUSED(status);
    // Now save all the translated priorities as our output...
    attrs->vplPrio = vplPrio;
    attrs->nativeSchedPolicy = nativePrio.policy;    // native pthread sched-policy
    
    attrs->nativeSchedPrio = nativePrio.params.__sched_priority;
}

static int UTLThread_NativeToUTLPrioMapHelper(int nativePrio)
{
    int pthreadMinPrio = 1;
    int pthreadMaxPrio = UTLTHREAD_PTHREAD_PRIO_MAX;
    int vplMinPrio = UTL_PRIO_MIN;
    int vplMaxPrio = UTL_PRIO_MAX;

    if (pthreadMaxPrio == pthreadMinPrio) {
        // Prevent division by zero later.
        return vplMinPrio;
    }

    // Check if it's out of range
    if ((nativePrio < pthreadMinPrio) || (nativePrio > pthreadMaxPrio)) {
        return VPL_ERR_INVALID;
    }
    // Priority is linearly mapped from the POSIX thread priority to UTL thread priority.
    double relativePrio = (double)(nativePrio - pthreadMinPrio) / (double)(pthreadMaxPrio - pthreadMinPrio);
    int vplPrio = vplMinPrio + (int)(relativePrio * (vplMaxPrio - vplMinPrio));
    return vplPrio;
} 

static void UTLThread_AttrsSetNativePrio(pthread_attr_t* nativeAttrs)
{
    UTLThread_attr_t vplAttrs;
    struct sched_param nativeSchedParams;

    UTLThread_AttrsGetPrio(&vplAttrs);
    memset(&nativeSchedParams, 0, sizeof(nativeSchedParams));
    nativeSchedParams.__sched_priority = vplAttrs.nativeSchedPrio;
    pthread_attr_setschedpolicy(nativeAttrs, vplAttrs.nativeSchedPolicy);
    pthread_attr_setschedparam(nativeAttrs, &nativeSchedParams);
}


static void UTLThread_MapAttrsToNativeAttrs(pthread_attr_t* nativeAttrs, const UTLThread_attr_t* vplAttrs)
{
    // 1. Map stack-size argument.
    // Suggest stack-size for new thread, via pthread attr.
    if (vplAttrs->stackSize != 0) {
        pthread_attr_setstacksize(nativeAttrs, vplAttrs->stackSize);
    } else {
        // Use the default stack size defined by UTL.
        pthread_attr_setstacksize(nativeAttrs, UTLTHREAD_STACKSIZE_DEFAULT);
    }
    // 2. Map detached state
    if (vplAttrs->createDetached) {
        pthread_attr_setdetachstate(nativeAttrs, PTHREAD_CREATE_DETACHED);
    }

    // 3. Map VPL priority onto native priority, which is a
    // (schedule policy, params) pair.
    if (vplAttrs->vplPrio == UTLTHREAD_PRIO_INHERIT) {
        // Just get the "native" pthread scheduler state, then
        // set that directly into the native attrs.
        UTLThread_AttrsSetNativePrio(nativeAttrs);
    } else {
        // We have an explicit VPl priority. Map that to native prio.
        UTLThread__prio_mapping_t nativePrio;
        memset(&nativePrio, 0, sizeof(nativePrio));
        // First, map the UTL priority to a (policy, params) pair.
        (void)UTLThread_MapPrioToNativePrio(&nativePrio, vplAttrs->vplPrio);
        // Then apply that (policy, params) pair to the native attrs.
        pthread_attr_setschedpolicy(nativeAttrs, nativePrio.policy);
        pthread_attr_setschedparam(nativeAttrs, &(nativePrio.params));
    }
    // Make sure that pthread_create() actually obeys the values in nativeAttrs.
    pthread_attr_setinheritsched(nativeAttrs, PTHREAD_EXPLICIT_SCHED);
}


UTLThreadId_t UTLDetachableThread_Self(void)
{
    return syscall(SYS_gettid);
}

int UTLThread_AttrInit(UTLThread_attr_t* attrs)
{
    if (attrs == NULL) {
        return VPL_ERR_INVALID;
    }
    // Documented VPL default stack-size.
    attrs->stackSize = UTLTHREAD_STACKSIZE_DEFAULT;
    attrs->createDetached = false;
    // Force us to get and use the calling thread's priority.
    attrs->vplPrio = UTLTHREAD_PRIO_INHERIT;
    // The other fields are currently not used, but we will set them anyway.
    attrs->nativeSchedPolicy = SCHED_OTHER;
    attrs->nativeSchedPrio = 0;
    attrs->stackAddr = NULL;
    return VPL_OK;
}

int UTLThread_AttrSetStackSize(UTLThread_attr_t* attrs, size_t stackSize)
{
    if (attrs == NULL) {
        return VPL_ERR_INVALID;
    }
    attrs->stackSize = stackSize;
    return VPL_OK;
}

int UTLThread_AttrDestroy(UTLThread_attr_t* attrs)
{
    if (attrs == NULL) {
        return VPL_ERR_INVALID;
    }
    memset(attrs, 0, sizeof(*attrs));
    return VPL_OK;
}

int UTLDetachableThread_Create(UTLDetachableThreadHandle_t* threadHandle_out,
    UTLDetachableThread_fn_t startRoutine,
    void* startArg,
    const UTLThread_attr_t* attrs,
    const char* threadName)
{
    int rc;
    int rv = VPL_OK;
    pthread_attr_t pthreadAttrs;
    pthread_t pthread;
    // Make a copy so we can modify it.
    UTLThread_attr_t myAttrs = utlThread__default_attrs;

    // unused parameters
    (void)threadName;

    // sanity checks
    if (startRoutine == NULL) {
        return VPL_ERR_INVALID;
    }
    // startArg may be NULL, so don't check it.
    // threadId_out may be NULL, so don't check it.

    // Map the VPLThread_attrs attributes onto pthread_attr_t attributes.
    if (attrs != NULL) {
        myAttrs = *attrs;
    }
    if (threadHandle_out == NULL) {
        if (!myAttrs.createDetached) {
            return VPL_ERR_INVALID;
        }
    } else {
        threadHandle_out->handle = 0;
    }
    // Late-bind any inheritance of the creating thread's priority.
    if (myAttrs.vplPrio == UTLTHREAD_PRIO_INHERIT) {
        myAttrs.vplPrio = UTLThread_GetSchedPriority();
    }
    pthread_attr_init(&pthreadAttrs);
    UTLThread_MapAttrsToNativeAttrs(&pthreadAttrs, &myAttrs);
    // TODO: do we expose {,non}inheritance of priority?
    {
        size_t tempStackSize;
        pthread_attr_getstacksize(&pthreadAttrs, &tempStackSize);
        UTL_LOG_INFO("Creating thread with stack size = "FMTu_size_t, tempStackSize);
    }
    rc = pthread_create(&pthread, &pthreadAttrs, startRoutine, startArg);
    if (rc != 0) {
        rv = VPLError_XlatErrno(rc);
        goto attr_destroy;
    }
    rv = VPL_OK;
    if (myAttrs.createDetached) {
        // Nothing else to do.
    } else {
        threadHandle_out->handle = pthread;
    }
    //if (threadId_out != NULL) {
        //    *threadId_out = pthread;
    //}
attr_destroy:
    pthread_attr_destroy(&pthreadAttrs);
    return rv;
}

int UTLThread_GetSchedPriority(void)
{
    int rv = -1;
    int policy;
    struct sched_param params;

    rv = pthread_getschedparam(pthread_self(), &policy, &params);
    if (rv != 0) {
        UTL_LOG_WARN("pthread_getschedparam failed with %d", rv);
        rv = VPLError_XlatErrno(rv);
        goto out;
    }
    // Special case: default POSIX thread priority.
    if (policy == SCHED_OTHER && params.sched_priority == 0) {
        rv = UTL_PRIO_MIN;
        goto out;
    }
    // Map POSIX thread priority to VPL thread priority
    rv = UTLThread_NativeToUTLPrioMapHelper(params.sched_priority);
out:
    return rv;
}

int UTLDetachableThread_Join(UTLDetachableThreadHandle_t* handle)
{
    int rc;
    int rv = VPL_OK;

    if (handle == NULL) {
        return VPL_ERR_INVALID;
    }
    if (handle->handle == 0) {
        return VPL_ERR_ALREADY;
    }
    if (handle->handle == pthread_self()) {
        return VPL_ERR_DEADLK;
    }

    rc = pthread_join(handle->handle, NULL);
    if (rc != 0) {
        rv = VPLError_XlatErrno(rc);
    } else {
        handle->handle = 0;
    }
    return rv;
} 

