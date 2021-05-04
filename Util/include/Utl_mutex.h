#ifndef __UTL_MUTEX_H__
#define __UTL_MUTEX_H__

#include "Utl_time.h"

// Magic numbers to reveal the state of a thread synchronization object.
#define UTL_TH_UNDEF   0xdeadc0de
#define UTL_TH_INIT    0x476f6f64 // Good
#define UTL_TH_UNINIT  0xdeadbeef

typedef struct {
    pthread_mutex_t rwlock;
} UTLSlimRWLock__t;

typedef UTLSlimRWLock__t UTLSlimRWLock_t;

typedef struct {
    pthread_mutex_t  pthMutex[1];
    pthread_t        lockTid;
    int              lockCount;
    unsigned int     magic;
} UTLMutex__t;

typedef UTLMutex__t  UTLMutex_t;

typedef struct {
    pthread_cond_t cond[1];
    unsigned int magic;
} UTLCond_t;

#define UTL_IS_INITIALIZED(obj) ((obj)->magic == UTL_TH_INIT)
#define UTL_SET_INITIALIZED(obj) (obj)->magic = UTL_TH_INIT
#define UTL_SET_UNINITIALIZED(obj) (obj)->magic = UTL_TH_UNINIT 

#define UTLIMPL_ASSERT(exp)  ((void)0)

int UTLMutex_Init(UTLMutex_t* mutex);
int UTLMutex_LockedSelf(const UTLMutex_t* mutex);
int UTLMutex_Lock(UTLMutex_t* mutex);
int UTLMutex_Unlock(UTLMutex_t* mutex);
int UTLMutex_Destroy(UTLMutex_t* mutex);

int UTLSlimRWLock_Init(UTLSlimRWLock_t* rwlock);
int UTLSlimRWLock_UnlockRead(UTLSlimRWLock_t* rwlock);
int UTLSlimRWLock_LockWrite(UTLSlimRWLock_t* rwlock);
int UTLSlimRWLock_UnlockWrite(UTLSlimRWLock_t* rwlock);
int UTLSlimRWLock_LockRead(UTLSlimRWLock_t* rwlock);

int UTLCond_Init(UTLCond_t* cond);
int UTLCond_Signal(UTLCond_t* cond);
int UTLCond_Broadcast(UTLCond_t* cond);
int UTLCond_TimedWait(UTLCond_t* cond, UTLMutex_t* mutex, UTLTime_t time);
int UTLCond_Destroy(UTLCond_t* cond);

#endif

