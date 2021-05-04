#ifndef __UTL_THREAD_H__
#define __UTL_THREAD_H__
#include "Comm.h"

#define UTLTHREAD_STACKSIZE_DEFAULT    (2*1024*1024)
#define UTLTHREAD_STACKSIZE_MIN    (32*1024)
#define UTLTHREAD_PRIO_INHERIT    (-1)

#define UTL_PRIO_MIN    (0)
#define UTL_PRIO_MAX    (9)
#define UTLTHREAD_PTHREAD_PRIO_MAX    (99)

#define UNUSED(x)  (void)(x)

typedef struct {
    pthread_t handle;
} UTLDetachableThreadHandle_t;

typedef struct  {
    size_t stackSize;
    bool createDetached;
    int vplPrio;
    int nativeSchedPolicy;
    int nativeSchedPrio;
    uint32_t* stackAddr;
} UTLThread_attr_t;

typedef struct {
    int policy;
    struct sched_param params;
} UTLThread__prio_mapping_t;


typedef uint64_t UTLThreadId_t;

#define UTLTHREAD__FN_ATTRS
typedef void* UTLDetachableThread__returnType_t;

typedef UTLDetachableThread__returnType_t (UTLTHREAD__FN_ATTRS *UTLDetachableThread_fn_t)(void*);
#define UTLTHREAD_FN_DECL  UTLDetachableThread__returnType_t UTLTHREAD__FN_ATTRS

UTLThreadId_t UTLDetachableThread_Self(void);
int UTLThread_AttrInit(UTLThread_attr_t* attrs);
int UTLThread_AttrSetStackSize(UTLThread_attr_t* attrs, size_t stackSize);
int UTLThread_AttrDestroy(UTLThread_attr_t* attrs);
int UTLDetachableThread_Create(UTLDetachableThreadHandle_t* threadHandle_out, UTLDetachableThread_fn_t startRoutine, void* startArg, const UTLThread_attr_t* attrs, const char* threadName);
int UTLThread_GetSchedPriority(void);
int UTLDetachableThread_Join(UTLDetachableThreadHandle_t* handle);

#endif
