/*************************************************************************
	> File Name    : skyeye_new_api.h
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2021/3/24 14:19:41
************************************************************************/

#ifndef _SKYEYE_NEW_API_H
#define _SKYEYE_NEW_API_H

#include <stdio.h>
#include <stdlib.h>
#include <skyeye_lock.h>
#include <skyeye_sched.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_types.h>
#include "skyeye_fault_injection.h"
#include <skyeye_core.h>
#include <skyeye_core_intf.h>
#include <skyeye_signal.h>
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_socket.h>
#include <fifo_queue.h>
#include "skyeye_system.h"
#include "parse_symbol.h"
#include "sim_control.h"
#include "skyeye_misc.h"
#include "lic.h"

#ifdef __WIN32__
#include <windows.h>
#endif

#define MAX_STRING_LEN 0x2000
#define MAX_PARM_NUM   10

typedef enum
{
    API_ERROR = 0,
    API_OK
}SKYEYE_API_RESULT;

//SkyEye C API接口返回通用结构体定义：
typedef struct
{
    SKYEYE_API_RESULT result;                          //API调用处理结果:API_OK为正确,API_ERROR为错误
    char              retStrValue[MAX_STRING_LEN];     //返回字符串类型的数据
    uint32_t          retIntValue;                     //返回uint32类型数据
    float             retFloatValue;                   //返回float类型数据
    double            retDoubleValue;                  //返回double类型数据
    uint64_t          retLongValue;                    //返回uint64类型数据
    conf_object_t     *conf_obj;                       //返回conf_object_t指针
    char              **list;                          //返回列表类型数据
    char              **errMsg;                        //返回错误码和错误信息参数
    dev_fi_t          *fault_inject;                   //返回故障注入结构体指针
}SkyEyeAPIRetST;

/*仿真控制API接口*/
SkyEyeAPIRetST skyeye_run();
SkyEyeAPIRetST skyeye_stop();
SkyEyeAPIRetST skyeye_quit();
SkyEyeAPIRetST skyeye_reset();
SkyEyeAPIRetST skyeye_running_status();
SkyEyeAPIRetST skyeye_prepare_running();
int prepare_to_run();

/*快照功能API接口*/
SkyEyeAPIRetST skyeye_load_checkpoint(char *checkpoint_path);
SkyEyeAPIRetST skyeye_save_checkpoint(char *checkpoint_path);
int load_chp(char *arg);
int save_chp(char *arg);

/*断点功能API接口*/
SkyEyeAPIRetST skyeye_create_breakpoint(char *cpuname, uint32_t addr);
SkyEyeAPIRetST skyeye_delete_breakpoint_by_id(char *cpuname, int id);
SkyEyeAPIRetST skyeye_delete_breakpoint_by_addr(char *cpuname, uint32_t address);
SkyEyeAPIRetST skyeye_get_breakpoint_address_by_id(char *cpuname, int id);
SkyEyeAPIRetST skyeye_get_bp_numbers(char *cpuname);
SkyEyeAPIRetST skyeye_check_bp_hit(char *cpuname);
SkyEyeAPIRetST skyeye_get_bp_hit_id(char *cpuname);
SkyEyeAPIRetST skyeye_clear_bp_hit(char *cpuname);

/*创建目标系统各模块API接口*/
SkyEyeAPIRetST skyeye_add_pre_obj(char *objname, char *classname);
SkyEyeAPIRetST skyeye_create_mach(char *objname, char *classname);
SkyEyeAPIRetST skyeye_create_linker(char *objname, char *classname);
SkyEyeAPIRetST skyeye_create_cpu(char *machname, char *objname, char *classname);
SkyEyeAPIRetST skyeye_create_device(char *machname, char *objname, char *classname);
SkyEyeAPIRetST skyeye_config_obj();
SkyEyeAPIRetST skyeye_config_module(char *objname);
int system_config_conf_obj();
SkyEyeAPIRetST skyeye_get_soc_num(void);
SkyEyeAPIRetST skyeye_get_cpu_num_by_soc_name(char *soc_name);
SkyEyeAPIRetST skyeye_get_current_system_module_name();

SkyEyeAPIRetST skyeye_add_map(char *memory_space_name, char *device_name, uint64_t address, uint32_t length);
SkyEyeAPIRetST skyeye_add_map_group(char *memory_space_name, char *device_name,uint64_t address, uint32_t length, uint32_t index);
int WIN_memory_space_add_map(char *memory_space_name, char *device_name, uint64_t address, uint32_t length);
int WIN_memory_space_add_map_group(char *memory_space_name, char *device_name, uint64_t address, uint32_t length, uint32_t index);


/*处理器相关API接口*/
SkyEyeAPIRetST skyeye_get_pc_by_cpuname(char *cpuname);
SkyEyeAPIRetST skyeye_step_run(char *cpuname, const char *arg);
SkyEyeAPIRetST skyeye_get_memory_value_by_cpuname(char *cpuname, char *addr); //该接口已不适用当前架构
SkyEyeAPIRetST skyeye_disassemble_by_addr(char *cpuname, uint32_t addr);
SkyEyeAPIRetST skyeye_get_cpu_address_width(char *cpuname);
SkyEyeAPIRetST skyeye_get_cpu_steps(char *cpuname);
SkyEyeAPIRetST skyeye_set_cpu_run_mode(char *cpuname, int mode);
SkyEyeAPIRetST skyeye_get_cpu_architecture(char *cpuname);
SkyEyeAPIRetST skyeye_get_simulation_time(char *cpu_name);
SkyEyeAPIRetST skyeye_get_simulation_insn_num(char *cpu_name);
SkyEyeAPIRetST skyeye_get_cpu_freq(char *cpu_name);

/*内存读写API接口*/
SkyEyeAPIRetST skyeye_memory_read(char *cpuname, uint32_t addr, int count);
SkyEyeAPIRetST skyeye_memory_write(char *cpuname, uint32_t addr, char *val, int count);
SkyEyeAPIRetST skyeye_device_read(char *device_name, uint32_t offset, int count);
SkyEyeAPIRetST skyeye_device_write(char *device_name, uint32_t offset, char *buf, int count);
SkyEyeAPIRetST skyeye_inject_device_read(char *device_name, char *data);
SkyEyeAPIRetST skyeye_inject_device_write(char *device_name, char *data, int size);

/*其他Command接口*/
SkyEyeAPIRetST skyeye_set_min_syn_time(double s, double delay);
void system_set_min_syn_time(double s, double delay);
SkyEyeAPIRetST skyeye_set_attr(char *objname, char *key, char *attr_type, char *value);
SkyEyeAPIRetST skyeye_connect_device(char *con_objname, char *iface_objname, char *name, uint32_t index);
SkyEyeAPIRetST skyeye_get_module_type(char *objname);
SkyEyeAPIRetST skyeye_get_class_list(void);
SkyEyeAPIRetST skyeye_get_class_type(char *classname);
SkyEyeAPIRetST skyeye_get_interface_list(char *classname);
SkyEyeAPIRetST skyeye_get_connect_list(char *classname);
SkyEyeAPIRetST skyeye_get_attr_list(char *classname);
SkyEyeAPIRetST skyeye_get_attr_info(char *classname, const char *attrname);
SkyEyeAPIRetST skyeye_get_module_names(void);
SkyEyeAPIRetST skyeye_get_device_module_type(char *classname);
SkyEyeAPIRetST skyeye_module_get_value_by_name(char *modulename, char *key);
SkyEyeAPIRetST skyeye_module_get_path_by_name(char *modulename);
SkyEyeAPIRetST skyeye_load_module_fromdir(char *dir);

SkyEyeAPIRetST skyeye_get_next_logMsg();
SkyEyeAPIRetST skyeye_list_dir(char *arg);
SkyEyeAPIRetST skyeye_list_modules(char *arg);
SkyEyeAPIRetST skyeye_get_mm_info(char *args);
SkyEyeAPIRetST skyeye_license_verify();
int check_cpu(char *cpuname);
SkyEyeAPIRetST skyeye_read_license_info(char *filename);
SkyEyeAPIRetST skyeye_get_cpuid();
SkyEyeAPIRetST skyeye_check_usbKey_connect();

SkyEyeAPIRetST skyeye_set_script_path(char *path);
SkyEyeAPIRetST skyeye_get_work_full_path();
SkyEyeAPIRetST skyeye_get_work_path();

/*数据激励功能API接口*/
SkyEyeAPIRetST skyeye_parse_symbol(char *cpuname, char *hex_file_name);
SkyEyeAPIRetST skyeye_debug_symbol();
SkyEyeAPIRetST skyeye_get_symbol_addr(char *cpu_name, char *sym_str);
SkyEyeAPIRetST skyeye_get_symbol_value(char *cpu_name, char *sym_str, sym_type_t count);
SkyEyeAPIRetST skyeye_get_float_symbol_value(char *cpu_name, char *sym_strvoid, sym_type_t count);
SkyEyeAPIRetST skyeye_get_double_symbol_value(char *cpu_name, char *sym_strvoid, sym_type_t count);
SkyEyeAPIRetST skyeye_set_symbol_value(char *cpu_name, char *sym_str, uint64_t value, sym_type_t count);
SkyEyeAPIRetST skyeye_set_float_symbol_value(char *cpu_name, char *sym_str, float value, sym_type_t count);
SkyEyeAPIRetST skyeye_set_double_symbol_value(char *cpu_name, char *sym_str, double value, sym_type_t count);
SkyEyeAPIRetST skyeye_get_func_addr(char *cpu_name, char *func_str);
SkyEyeAPIRetST skyeye_get_func_length(char *cpu_name, char *func_str);

/*寄存器API接口*/
SkyEyeAPIRetST skyeye_get_register_num(char *devicename);
SkyEyeAPIRetST skyeye_get_regname_by_id(char *devicename, uint32_t id);
SkyEyeAPIRetST skyeye_get_regvalue_by_id(char *devicename, uint32_t id);
SkyEyeAPIRetST skyeye_get_regoffset_by_id(char *devicename, uint32_t id);
SkyEyeAPIRetST skyeye_set_regvalue_by_id(char *devicename, uint64_t value, uint32_t id);
SkyEyeAPIRetST skyeye_get_regid_by_name(char *devicename, char *name);

/*GDB调试功能API接口*/
SkyEyeAPIRetST skyeye_create_remote_gdb(char *cpuname, int port, char *ip);
SkyEyeAPIRetST skyeye_delete_remote_gdb(char *cpuname);
SkyEyeAPIRetST skyeye_remote_gdb_check_link(char *cpuname);
SkyEyeAPIRetST skyeye_remote_gdb_get_client_ip(char *cpuname);

/*加载功能API接口*/
SkyEyeAPIRetST skyeye_load_file(char *cpuname, const char *filename, generic_address_t load_addr);
SkyEyeAPIRetST skyeye_load_binary(char *cpuname, char *elfname);
SkyEyeAPIRetST skyeye_load_bin_binary(char *cpuname, const char *filename, generic_address_t load_addr, int length, int start_pc);
SkyEyeAPIRetST skyeye_new_load_file(char *memoryspace_name, const char *filename, generic_address_t load_addr);

/*故障注入功能API接口*/
SkyEyeAPIRetST skyeye_set_fault(char *memory_space_name, uint32_t addr, uint32_t bit, uint32_t mode, uint32_t count);
SkyEyeAPIRetST skyeye_get_fault(char *memory_space_name, uint32_t addr);
SkyEyeAPIRetST skyeye_clear_fault(char *memory_space_name, uint32_t addr, uint32_t bit, uint32_t mode);

/*代码覆盖率功能API接口*/
SkyEyeAPIRetST skyeye_get_executed_pc_file(char *instr_process_name, char *filename);
SkyEyeAPIRetST skyeye_get_instr_process_device(char *machname, char *cpuname, char *classname);
SkyEyeAPIRetST skyeye_enable_cpu_codecov(char *machname);

/*指令流记录功能API接口*/
SkyEyeAPIRetST skyeye_get_pc_record_size(char *instr_process_name);
SkyEyeAPIRetST skyeye_set_pc_record_size(char *instr_process_name, int size);
SkyEyeAPIRetST skyeye_get_pc_record_nums(char *instr_process_name);
SkyEyeAPIRetST skyeye_get_pc_record_index(char *instr_process_name);
SkyEyeAPIRetST skyeye_get_pc_record_overflow(char *instr_process_name);
SkyEyeAPIRetST skyeye_get_pc_by_index(char *instr_process_name, int id);
SkyEyeAPIRetST skyeye_open_instr_record(char *cpu_name, char *filename);
SkyEyeAPIRetST skyeye_close_instr_record(char *cpu_name);

/*异常触发功能API接口*/
SkyEyeAPIRetST skyeye_get_cpu_exception_num(char *cpu_name);
SkyEyeAPIRetST skyeye_set_cpu_exception(char *cpu_name, int exception_id);
SkyEyeAPIRetST skyeye_set_ext_interrupt(char *intc_name, int interrupt_num);
SkyEyeAPIRetST skyeye_get_cpu_exception_name_by_id(char *cpu_name, int exception_id);

/*使能和禁用设备API接口*/
SkyEyeAPIRetST skyeye_disable_device_work(char *device_name);
SkyEyeAPIRetST skyeye_enable_device_work(char *device_name);

/*设置和获取CPU PC API接口*/
SkyEyeAPIRetST skyeye_set_pc(char *cpu_name, unsigned int pc);
SkyEyeAPIRetST skyeye_get_pc(char *cpu_name);

/*enable SkyEye debug iface*/
SkyEyeAPIRetST skyeye_enable_debug(char *soc_name);

/*等待Term终端输出某个字符串 API接口*/
SkyEyeAPIRetST skyeye_term_wait_for_string(char *termname, const char *string);
/*向Term终端写入某个字符串 API接口*/
SkyEyeAPIRetST skyeye_term_write(char *termname, const char *string);
/*当Term终端输出某个字符串则写入某个字符串 API接口*/
SkyEyeAPIRetST skyeye_term_wait_then_write(char *termname, const char *wait_string, const char *write_string);

int WIN_set_attr(char *objname, char *key, char *attr_type, char *value);
int WIN_connect(char *con_objname, char *iface_objname, char *name, uint32_t index);
char **get_class_list(void);
char *get_class_type(char *classname);
char **get_interface_list(char *classname);
char **get_connect_list(char *classname);
const char **get_attr_list(char *classname);
const char **get_attr_info(char *classname, const char *attrname);
char *WIN_get_module_names(void);
int get_device_module_type(char *classname);
char *SKY_module_get_value_by_name(char *modulename, char *key);
char *SKY_module_get_path_by_name(char *modulename);
void SKY_load_all_modules(char *lib_dir, char *suffix);
char *skyeye_get_next_log();
int com_list(char *arg);
int com_list_modules(char *arg);
void mm_info_cmd(char *args);
int license_verify();
const char *read_lic_info(char *filename);
const bool_t check_usb_lic_connect();
int WIN_set_script_path(char *path);
char *WIN_get_work_full_path();
char *WIN_get_work_path();
void parse_symbol_xml(char *cpu_name, char *xml_filename);
int SkyEye_GetSymbol_Addr(char *cpu_name, char *sym_str, uint32_t * global_addr);
uint64_t SkyEye_GetSymbol_Value_By_Py(char *cpu_name, char *sym_str, sym_type_t count);
float SkyEye_GetFloatSymbol_Value(char *cpu_name, char *sym_strvoid, sym_type_t count);
double SkyEye_GetDoubleSymbol_Value(char *cpu_name, char *sym_strvoid, sym_type_t count);
int SkyEye_SetSymbol_Value_By_Py(char *cpu_name, char *sym_str, uint64_t value, sym_type_t count);
int SkyEye_SetSymbol_Value(char *cpu_name, char *sym_str, void *value, sym_type_t count);
uint32_t SkyEye_GetFunc_Addr(char *cpu_name, char *func_str);
uint32_t SkyEye_GetFunc_Length(char *cpu_name, char *func_str);
int WIN_create_remote_gdb(char *cpuname, int port, char *ip);
int WIN_delete_remote_gdb(char *cpuname);
int WIN_remote_gdb_check_link(char *cpuname);
char *WIN_remote_gdb_get_client_ip(char *cpuname);

exception_t load_file(conf_object_t * obj, const char *filename, generic_address_t load_addr);
bool_t is_elf(const char *filename);
bool_t is_coff(const char *filename);
bool_t is_hex(const char *filename);
exception_t get_elf_start_addr(const char *file, generic_address_t * addr, generic_address_t * code_start, generic_address_t * code_end);
exception_t get_coff_start_addr(const char *filename, generic_address_t * start_addr, generic_address_t * code_start_addr, generic_address_t * code_end_addr);
exception_t get_hex_start_addr(const char *filename, generic_address_t * start_addr, generic_address_t * code_start_addr, generic_address_t * code_end_addr);

void set_entry_to_cpu(conf_object_t * core, uint32_t entry_point);
void SIM_set_pc(conf_object_t * conf_obj, generic_address_t pc, generic_address_t code_start, generic_address_t code_end);
exception_t SKY_load_file(conf_object_t * obj, memory_space_intf * memory_space, const char *filename);

int WIN_enable_cpu_codecov(char *machname);
char *get_instr_process_device(char *machname, char *cpuname, char *classname);

int WIN_open_instr_record(char *cpu_name, char *filename);
int WIN_close_instr_record(char *cpu_name);

int SkyEye_GetCpuExceptionNum(char *cpu_name);
exception_t SkyEye_SetCpuException(char *cpu_name, int exception_id);
exception_t SkyEye_SetExtInterrupt(char *intc_name, int interrupt_num);
char *SkyEye_GetCpuExceptionNameById(char *cpu_name, int exception_id);
int WIN_disable_device_work(char *device_name);
int WIN_enable_device_work(char *device_name);
int WIN_setPC(char *cpu_name, unsigned int pc);
uint32_t WIN_getPC(char *cpu_name);

char **get_current_system_module_name();
int parse_xml_output();
#endif
