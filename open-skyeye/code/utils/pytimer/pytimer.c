#include "pytimer.h"
#include "skyeye_system.h"
#include "skyeye_mm.h"
#include "skyeye_obj.h"
#include "skyeye_log.h"
#include "memory_monitor.h"
#include "skyeye_api.h"

#ifdef __WIN32__
#include <windows.h>
#endif

static void PyTmrCallback(void *, time_data_t *);
static void PyPcWatchCallback(void *data);
static void PyMemoryWatchCallback(void *user_data);

/*
 ***********************************************************************************************
 *                              CREATE A TIMER FROM PYTHON
 * Destription:  This function is called from python to create timer callback in simulation
 *
 * Argument(s):  mode   Is mode of this timer
 * 
 * Return(s)  :  None
 *
 * Note(s)    :  None
 *
 ***********************************************************************************************
 */
PYTMR *PyTmrCreate(conf_object_t * cpu, TM ms, PYTMR_MODE_T mode)
{
    PYTMR *pytmr;
    pytmr = skyeye_mm_zero(sizeof (struct pytmr));
    pytmr->Mode = mode;
    pytmr->cpu = cpu;
    RWLOCK_INIT(pytmr->Lock);

    conf_object_t *idle_device = ((sys_cpu_t *) cpu->sys_struct)->sys_soc->idle_device->dev;

    uint32_t flags = SCHED_MS;

    switch (mode)
    {
        case PYTMR_ONE_SHOT:
            flags |= SCHED_ONCE;
            break;
        case PYTMR_PERIOD:
            flags |= SCHED_NORMAL;
            break;
        default:
            return (PYTMR *) 0u;
    }
    pytmr->TmrHandle = system_register_timer_handler_with_data(idle_device, pytmr, ms, PyTmrCallback, flags);
    return pytmr;
}

PYTMR *PySetWatchOnPc(char *cpu_name, uint32_t pc_addr)
{
    PYTMR *pytmr;
    conf_object_t *obj = get_conf_obj(cpu_name);

    if (NULL == obj)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu_name);
        return 0;
    }
    pytmr = skyeye_mm_zero(sizeof (struct pytmr));
    pytmr->cpu = obj;
    RWLOCK_INIT(pytmr->Lock);

    SkyEye_SetWatchOnPC(cpu_name, pc_addr, PyPcWatchCallback, pytmr);
    return pytmr;
}

PYTMR *PySetWatchOnMemory(char *ms_name, int mm_type, uint32_t m_addr, uint32_t data_type, uint32_t m_length)
{
    PYTMR *pytmr;
    conf_object_t *mm_obj = get_conf_obj(ms_name);

    if (NULL == mm_obj)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get mm_obj for %s.\n", ms_name);
        return 0;
    }
    pytmr = skyeye_mm_zero(sizeof (struct pytmr));
    pytmr->mm_space = mm_obj;
    RWLOCK_INIT(pytmr->Lock);

    /*Enable dyncom memory watch */
    SkyEye_SetDyncomWatchMemoryStatus(m_addr, 1);

    Py_SetWatchOnMemory(mm_obj, ms_name, mm_type, m_addr, data_type, m_length, PyMemoryWatchCallback, pytmr);
    return pytmr;
}

/*
 ***********************************************************************************************
 *                              SYSTEM TIME CALLBACK
 * Destription:  This function is called when the time reach
 *
 * Argument(s):  arg    Is idle_device object pointer
 *              data    Is timer data pointer
 * 
 * Return(s)  :  None
 *
 * Note(s)    :  None
 *
 ***********************************************************************************************
 */

void PyMemoryWatchCallback(void *data)
{
    PYTMR *pytmr = (PYTMR *) (data);

    pytmr->Hit = True;
    while(!pytmr->isSync)
    {
#ifdef __WIN32__
        Sleep(1);
#else
        usleep(1);
#endif
    }
    pytmr->isSync = False;
}

void PyTmrCallback(void *arg, time_data_t * data)
{
    PYTMR *pytmr = (PYTMR *) (data->user_data);

    pytmr->Hit = True;
    while(!pytmr->isSync)
    {
#ifdef __WIN32__
        Sleep(1);
#else
        usleep(1);
#endif
    }
    pytmr->isSync = False;
}

void PyPcWatchCallback(void *data)
{
    PYTMR *pytmr = (PYTMR *) (data);

    pytmr->Hit = True;
    while(!pytmr->isSync)
    {
#ifdef __WIN32__
        Sleep(1);
#else
        usleep(1);
#endif
    }
    pytmr->isSync = False;
}

uint32_t PythonTmrCreate(char *cpu, TM ms, PYTMR_MODE_T mode)
{
    PYTMR *p_pytmr;
    conf_object_t *obj = get_conf_obj(cpu);

    if (NULL == obj)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not get obj for %s.\n", cpu);
        return 0;
    }
    p_pytmr = PyTmrCreate(obj, ms, mode);
    return (uint32_t) p_pytmr;
}

uint32_t PythonSetWatchOnPc(char *cpuname, uint32_t pc_addr)
{
    PYTMR *p_pytmr;

    p_pytmr = PySetWatchOnPc(cpuname, pc_addr);
    return (uint32_t) p_pytmr;
}

void PythonUnWatchOnPc(char *cpuname, uint32_t pc_addr)
{
    SkyEye_UnWatchOnPc(cpuname, pc_addr);
}

int PythonSetWatchonMemory(char *ms_name, int mm_type, uint32_t m_addr, uint32_t data_type, uint32_t m_length)
{
    PYTMR *p_pytmr;

    p_pytmr = PySetWatchOnMemory(ms_name, mm_type, m_addr, data_type, m_length);
    return (uint32_t) p_pytmr;
}

int PythonUnWatchOnMemory(char *ms_name, int mm_type, uint32_t m_addr, uint32_t m_length)
{
    PYTMR *p_pytmr;

    p_pytmr = SkyEye_UnWatchOnMemory(ms_name, mm_type, m_addr, m_length);
    return (int) p_pytmr;
}

void PythonTmrProcessOk(uint32_t ptr)
{
    PYTMR *p_pytmr = (PYTMR *) ptr;

    p_pytmr->isSync = True;
    RW_UNLOCK(p_pytmr->Lock);
}

bool_t PythonTmrCheckHit(uint32_t ptr)
{
    PYTMR *p_pytmr = (PYTMR *) ptr;

    if (p_pytmr->Hit)
    {
        p_pytmr->Hit = False;
        return True;
    } else
        return False;
}

bool_t PythonTmrDelete(uint32_t ptr)
{
    PYTMR *p_pytmr = (PYTMR *) ptr;

    return system_del_timer_handler(p_pytmr->TmrHandle);
}
