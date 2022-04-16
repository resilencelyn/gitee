#ifndef __COMMON_SKYEYE_API_H__
#define __COMMON_SKYEYE_API_H__
#define SKYEYE_API_VERSION 1.0

#include <skyeye_types.h>
#include <skyeye_system.h>
#include <memory_monitor.h>
#include <skyeye_arch.h>

typedef void(*Sync_CallBack) (void);

uint64_t get_simulation_insn_number(char *cpu_name, conf_object_t *conf_obj);
double WIN_get_simulation_time(char *cpu_name, conf_object_t *obj);
int WIN_set_script_path(char *path);
char *WIN_get_work_full_path();
char *WIN_get_work_path();
uint32_t WIN_get_CPU_PC(char *cpu_name, conf_object_t *conf_obj);
uint32_t WIN_read_address_bus(char *cpu_name, conf_object_t *conf_obj, uint32_t addr, void *value, uint32_t count);
uint32_t WIN_write_address_bus(char *cpu_name, conf_object_t *conf_obj, uint32_t addr, void *value, uint32_t count);
int32_t SkyEye_CreateTimer(char *devicename,void *user_data,uint64_t time,time_sched_t func,uint32_t flags);
bool_t SkyEye_DeleteTimer(time_handle_t handle);
double SkyEye_GetRunTime(char *cpu_name,conf_object_t *obj);
uint32_t SkyEye_GetCurPC(char *cpu_name,conf_object_t *obj);
bool_t SkyEye_IsCpuStopped(void);
bool_t SkyEye_IsCpuStopped(void);
char *SkyEye_GetPrjPath(void);
void SkyEye_StartDebug(void);
void SkyEye_StopDebug(generic_arch_t* arch_instance);
void SkyEye_ResetDebug(void);
void SkyEye_ResetCpu(void);
int SkyEye_SetWatchOnMemory(conf_object_t *conf_obj,char *ms_name,int mm_type,uint32_t m_addr,uint32_t m_length,\
		OP_DataType_t data_type,cond_compare_t cond,void *value,memory_callback_t callback);
int SkyEye_UnWatchOnMemory(char *ms_name,int mm_type,uint32_t m_addr,uint32_t m_length);
int SkyEye_OutCovFile(char *instr_process_name,char *fname);
uint8_t SkyEye_ReadByte(char *cpu_name,uint32_t mm_addr);
uint32_t SkyEye_ReadDWord(char *cpu_name,uint32_t mm_addr);
int SkyEye_WriteByte(char *cpu_name,uint32_t mm_addr,uint8_t value);
int SkyEye_WriteDWord(char *cpu_name,uint32_t mm_addr,uint32_t value);
uint64_t SkyEye_GetCpuFreq(char *cpu_name, conf_object_t *conf_obj);
void SkyEye_ClearCov(char *fname);
void SkyEye_ClearLog();
void SkyEye_SetWatchOnPC(char *cpu_name,uint32_t pc_addr,PC_Watch_CallBack func,void *user_data);
void SkyEye_UnWatchOnPc(char *cpu_name,uint32_t pc_addr);
void SkyEye_RegisterSyncCallBack(Sync_CallBack func);
int SkyEye_GetCpuExceptionNum(char *cpu_name);
exception_t SkyEye_SetCpuException(char *cpu_name, int exception_id);
exception_t SkyEye_SetExtInterrupt(char *intc_name, int interrupt_num);
char* SkyEye_GetCpuExceptionNameById(char *cpu_name, int exception_id);
int SkyEye_SetCpuRunMode(char *cpu_name, int run_mode);
#define SkyEye_AddLog skyeye_log
#endif
