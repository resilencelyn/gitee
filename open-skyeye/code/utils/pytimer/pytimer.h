#ifndef __PYTIMER__
#define __PYTIMER__
#include "skyeye_types.h"
#include "skyeye_lock.h"
#include "skyeye_system.h"
#include "mm_monitor.h"

typedef struct pytmr PYTMR;
typedef uint32_t PYTMR_ID;
typedef uint32_t TM;

typedef enum
{
    PYTMR_ONE_SHOT,
    PYTMR_PERIOD,
} PYTMR_MODE_T;

typedef struct pytmr
{
    PYTMR_MODE_T Mode;
    bool_t Hit;
    bool_t isSync;
    PYTMR *NextPtr;
    PYTMR *PrevPtr;
    RWLOCK_T Lock;
    conf_object_t *cpu;
    conf_object_t *mm_space;
    time_handle_t TmrHandle;

};
#endif
