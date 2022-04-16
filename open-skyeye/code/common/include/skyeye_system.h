#ifndef SKYEYE_SYSTEM
#define SKYEYE_SYSTEM

#ifdef __cplusplus
 extern "C" {
#endif


#include "skyeye_sched.h"
#include "skyeye_core_intf.h"
#include "skyeye_queue.h"
#include "skyeye_lock.h"

#define  MAX_TIMER 64
#define  MAX_CPU 16
#define  MAX_DEV 64
#define  MAX_SOC 32
#define MAX_LINKER 64
#define MAX_NAME_LEN 256
#define MAX_MODULE_NUM   100

#define TIME_S 0
#define  TIME_MS 1
#define  TIME_US 2
#define  TIME_NS 3


#define SCHED_NORMAL 0x0
#define SCHED_ONCE 0x1
#define SCHED_S   (0x0 << 1)
#define SCHED_MS  (0x1 << 1)
#define SCHED_US  (0x2 << 1)
#define SCHED_NS  (0x3 << 1)
#define NORMAL_FLAGS   (SCHED_MS | SCHED_NORMAL)

#define MAX_WATCH_NUM 1024

#define NO_SYNCHRONIZATION 0
#define SYNCHRONIZATION 1

typedef int32_t time_handle_t;

typedef struct sys_soc sys_soc_t;

typedef struct{
     uint64_t pass_cycle;
     void *user_data;
}time_data_t;

typedef void(*time_sched_t)(void *obj, time_data_t *data);

typedef struct{
	conf_object_t *target;
    time_sched_t func;
    void *user_data;
	bool_t enable;
    bool_t status;
    uint32_t sched_type;
	uint32_t be_sched_count;
    uint64_t time;
    uint32_t time_type;
    uint64_t sched_cycle;

}sys_timer_t;

struct event{
	conf_object_t *target;
	uint64_t cycle_count;
	uint64_t pass_count;
	sys_timer_t *timer;
	LIST_ENTRY (event)list_entry;	
};

typedef void(*PC_Watch_CallBack)(void *obj);

typedef struct{
	uint32_t addr;
	char enable;
	PC_Watch_CallBack pc_watch_func;
	void *pc_watch_data;
}pc_watch;

typedef struct{
	conf_object_t *cpu;
	char *sys_name;
	sys_soc_t *sys_soc;
	uint32_t freq_hz;  //HZ
	core_exec_intf* core_exec;
    core_info_intf* core_info;
	uint32_t id;
	uint64_t step_sum;
	pc_watch *(pc_watch_info[MAX_WATCH_NUM]);
	uint32_t pc_watch_count; 
	/*Set the var for the x86 architecture*/
	bool_t is_x86_arch;
	uint64_t last_usec;

	uint32_t syn_b; 

	uint64_t io_syn_pass_count;
 
	/*Set the var for the x86 architecture*/
	uint64_t io_syn_pass_usec;

	uint64_t io_syn_count;

	uint64_t delay_syn_count;
}sys_cpu_t;

typedef struct {
	conf_object_t *dev;
	sys_soc_t *sys_soc;
}sys_dev_t;

typedef struct {
	conf_object_t *linker;
}sys_linker_t;


struct sys_soc{
	conf_object_t *soc;
	sys_dev_t devs[MAX_DEV];
	sys_cpu_t cpus[MAX_CPU];
	sys_timer_t *timers[MAX_TIMER];

	/*The number of time handler included in the soc*/
	uint32_t timer_cnt;
	/*The number of devicevs included in the soc
	 */
	uint32_t dev_cnt;
	/*The number of cpus included in the soc
	  */
	uint32_t cpu_cnt;

	uint64_t io_cycle_count;
    
	LIST_HEAD(timer_handler_head, event) handler_list;

	sys_dev_t *idle_device;

	sys_dev_t *memory_space[MAX_DEV];

	uint32_t mem_cnt;

	char binary_path[MAX_NAME_LEN];
	/*After some time, the cpu on this soc begins to execute
	 **/
	uint32_t delay_load_ms;
	bool_t isEnableInstrProcessDevice;

	bool_t loaded;

};

typedef struct{
	sys_timer_t timers[MAX_TIMER];
	sys_soc_t socs[MAX_SOC];
	sys_linker_t linkers[MAX_LINKER];
	uint32_t timer_cnt;
	uint32_t soc_cnt;
	uint32_t linker_cnt;
	uint32_t dispatch_ready;
    double synchronization_time;
	uint32_t syn_enable;
	uint32_t delay_syn;
	uint32_t syn; 
	uint32_t syn_runtime; 
	uint64_t syn_time_us;
	uint64_t delay_syn_time_us;
	RWLOCK_T lock;
	bool_t   all_soc_loaded;
}skyeye_system_t;

exception_t system_clock_calculation(void);

skyeye_system_t *system_get();

void *system_get_struct(conf_object_t *obj);

time_handle_t _system_register_timer_handler(conf_object_t* obj, void *user_data, uint64_t time, time_sched_t func, uint32_t flags);

#define system_register_timer_handler(obj, time, callback, flags)\
	_system_register_timer_handler( obj, NULL,time, callback, flags)

#define system_register_timer_handler_with_data(obj, user_data, time, callback, flags)\
	_system_register_timer_handler(obj, user_data, time, callback, flags)


bool_t system_del_timer_handler(time_handle_t handle);
bool_t system_disable_timer(time_handle_t handle);
bool_t system_enable_timer(time_handle_t handle);

sys_cpu_t *system_register_cpu(conf_object_t *cpu, conf_object_t *soc);

sys_dev_t *system_register_dev(conf_object_t *dev, conf_object_t *soc);

sys_linker_t *system_register_linker(conf_object_t *linker);

sys_soc_t *system_register_soc(conf_object_t *soc);
void system_cpu_cycle(sys_cpu_t *cpu);

exception_t system_clock_calculation(void);

void system_clear();

void system_init();

#ifdef __cplusplus
}
#endif


#endif


