#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#include <skyeye_log.h>
#include <skyeye_dev_bus_intf.h>
#include <skyeye_dev_intf.h>
#include <skyeye_uart_ops.h>
#include <skyeye_system.h>
#define DEBUG
#include <skyeye_api.h>
#include "interface_test.h"

static int count;
static void watch_pc_test(void *obj)
{
	uint32_t pc_value = SkyEye_GetCurPC(NULL,obj);
	printf("[SkyEye_SetWatchOnPC 1 TEST]     Watch PC : 0x%x\n",pc_value);
}

static void dev_watch_pc_test(void *obj)
{
	interface_test_device *dev = ((conf_object_t *)obj)->obj;
	printf("[SkyEye_SetWatchOnPC 2 TEST]   dev->IsCpuStopped: 0x%x\n",dev->IsStop);
}

static void SyncFunc()
{
	uint32_t addr = 0x1dfc00;
	uint32_t _32_value = 0x8001;
	uint32_t tmp = 0;
	SkyEye_WriteDWord("e300_core_0",addr,_32_value);	
	tmp = SkyEye_ReadDWord("e300_core_0",addr);
	printf("[SkyEye_RegisterSyncCallBack TEST] write value : 0x%x,read value : 0x%x\n",_32_value,tmp);
}

static void Is_Run(conf_object_t *obj)
{
	interface_test_device *dev = (interface_test_device *)obj->obj;
	dev->IsStop = SkyEye_IsCpuStopped();
	if(dev->IsStop == 0)
		printf("[SkyEye_IsCpuStopped TEST]    dev->IsCpuStopped : 0x%x means not stop!\n",dev->IsStop);
	count++;
	if(count == 5){
		SkyEye_StopDebug(NULL);
		dev->IsStop = SkyEye_IsCpuStopped();
		if(dev->IsStop == 1)
			printf("[SkyEye_IsCpuStopped TEST]    dev->IsCpuStopped : 0x%x means stop!\n",dev->IsStop);
	}
}

static void memory_callback(conf_object_t *con_obj,uint32_t mm_type,uint32_t addr,void *val,size_t count)
{
	printf("[SkyEye_SetWatchOnMemory TEST] Watch On Memory addr : 0x%x\n",addr);
}

#if 1
void Timer_Func(conf_object_t *obj)
{
	interface_test_device *dev = (interface_test_device *)obj->obj;
	if(dev->count < 10)
	{
		dev->SimTime = SkyEye_GetRunTime(NULL,obj);
		printf("[SkyEye_CreateTimer TEST]   count : %d\n",dev->count);
		printf("[SkyEye_GetRunTime TEST]    dev->SimTime : %.6f\n",dev->SimTime);
	}
	else
	{	
		uint8_t _bit8 = 0xc3;
		uint32_t _bit32 = 0x432dd234;
#if 0
		uint64_t cpu_cycle;

		cpu_cycle = get_simulation_insn_number("e300_core_0",NULL);
		printf("[get_simulation_insn_number TEST]  cpu_cycle : 0x%x\n",cpu_cycle);
#endif		
		SkyEye_RegisterSyncCallBack(SyncFunc);
		printf("[SkyEye_DeleteTimer TEST]    stop timer Callback Function\n");
		SkyEye_DeleteTimer(dev->handle);	
		dev->count = 1;

		dev->pc = SkyEye_GetCurPC(NULL,obj);
		dev->IsStop = SkyEye_IsCpuStopped();
		char *script_path = SkyEye_GetPrjPath ();
		if(dev->IsStop == 1)
			printf("[SkyEye_IsCpuStopped TEST]    dev->IsCpuStopped : 0x%x means stop!\n",dev->IsStop);
		printf("[SkyEye_GetCurPC TEST]    dev->pc : 0x%x\n",dev->pc);
		printf("[SkyEye_GetPrjPath TEST]    script file path : %s\n",script_path);

		printf("[SkyEye_WriteByte TEST]    write byte value : 0x%x\n",_bit8);
		SkyEye_WriteByte("e300_core_0",0x236d50,_bit8);
		printf("[SkyEye_WriteWord TEST]    write word value : 0x%x\n",_bit32);
		SkyEye_WriteDWord("e300_core_0",0x236d54,_bit32);
	
		_bit8 = SkyEye_ReadByte("e300_core_0",0x236d50);
		printf("[SkyEye_ReadByte TEST]    read byte value : 0x%x\n",_bit8);
		_bit32 = SkyEye_ReadDWord("e300_core_0",0x236d54);
		printf("[SkyEye_ReadWord TEST]    read word value : 0x%x\n",_bit32);

#if 0	
		printf("[SkyEye_ClearLog TEST]  Clear the log\n");
		SkyEye_ClearLog();
#endif
		dev->pc = SkyEye_GetCurPC(NULL,obj);	
		SkyEye_AddLog(Info_log,__FUNCTION__,"current PC : 0x%x\n",dev->pc);
		printf("[SkyEye_AddLog TEST]    write current pc to log\n");
		SkyEye_StopDebug(NULL);
		printf("[SkyEye_StopDebug TEST]     now SkyEye is Stop,press any key to continue\n");
		
		getchar();
		printf("[SkyEye_StartDebug TEST] start SkyEye\n");
		SkyEye_StartDebug();
	
		printf("====SkyEye API TEST END====\n");
		dev->count = -1;
	}
	dev->count ++;
}
#endif


// Register read operation method implementation
static exception_t interface_test_read(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	interface_test_device *dev = (interface_test_device *)conf_obj->obj;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in interface_test\n", offset);
			break;
	}
	return No_exp;
}

// Register write operation method implementation
static exception_t interface_test_write(conf_object_t *conf_obj, generic_address_t offset, void *buf, size_t count)
{
	interface_test_device *dev = (interface_test_device *)conf_obj->obj;
	uint32_t val = *(uint32_t *)buf;
	switch (offset)
	{
		default:
			skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in interface_test\n", offset);
			break;
	}
	return No_exp;
}

// Register interface: gets the register name based on the register ID
static char* interface_test_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

// Register interface: set register value according to register ID
static exception_t interface_test_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	interface_test_device *dev = (interface_test_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

// Register interface: gets how many registers the device has
static uint32_t interface_test_get_regnum(conf_object_t *conf_obj)
{
	return  sizeof(struct registers) / 4;
}

// Register interface: gets the register value based on the register ID
static uint32_t interface_test_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	interface_test_device *dev = (interface_test_device *)conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

// Register interface: gets the register ID based on the register name
static uint32_t interface_test_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

// A new device object is instantiated, and obj_name is the name configured in the json file
static conf_object_t* new_interface_test(const char *obj_name)
{
	interface_test_device* dev = skyeye_mm_zero(sizeof(interface_test_device));
	dev->obj = new_conf_object(obj_name, dev);
	// TODO: place the code here

	// Register initialization
	struct registers *regs = &(dev->regs);
	return dev->obj;
}

// Configure the instantiated device object
static exception_t config_interface_test(conf_object_t* conf_obj)
{
	printf("======SkyEye API TEST======\n");
	interface_test_device *dev = (interface_test_device *)conf_obj->obj;
	// TODO: place the code here

	dev->handle = SkyEye_CreateTimer("interface_test_0",NULL,20,(time_sched_t)Timer_Func,SCHED_MS | SCHED_NORMAL);
	SkyEye_CreateTimer("eTSEC_0",NULL,500,(time_sched_t)Is_Run,SCHED_MS | SCHED_NORMAL);
	SkyEye_SetWatchOnPC("e300_core_0",0x1a44c,watch_pc_test,conf_obj);
	SkyEye_SetWatchOnPC("e300_core_0",0x1a450,dev_watch_pc_test,conf_obj);
#if 1
	SkyEye_UnWatchOnPc("e300_core_0",0x1a450);
#endif
	SkyEye_SetWatchOnMemory(conf_obj,"memory_space_1",MO_READ_OP | MO_WRITE_OP | MO_COND_OP, \
				0x4500,64,OP_UINT32,No_Cond,NULL,&memory_callback);
	SkyEye_UnWatchOnMemory("memory_space_1", MO_READ_OP | MO_WRITE_OP | MO_COND_OP, 0x4500,64);
	No_exp;
}

// Free the device object to do some memory free
static exception_t free_interface_test(conf_object_t* conf_obj)
{
	interface_test_device *dev = (interface_test_device *)conf_obj->obj;
	skyeye_free(conf_obj->objname);
	skyeye_free(conf_obj->obj);
	skyeye_free(conf_obj);
	return No_exp;
}

// initializes the module class and executes the following code when the module is loaded by SkyEye
void init_interface_test(void)
{
	// register a device with SkyEye
	static skyeye_class_t class_data =
	{
		.class_name      = "interface_test",
		.class_desc      = "",
		.new_instance    = new_interface_test,
		.free_instance   = free_interface_test,
		.config_instance = config_interface_test,
		.module_type     = SKYML_OFFCHIP,
	};
	conf_class_t* dev_class = SKY_register_device_class(class_data.class_name, &class_data);
	
	// sets the register address read-write method
	static memory_space_intf io_memory_interface =
	{
		.read  = interface_test_read,
		.write = interface_test_write
	};
	SKY_register_iface(dev_class, MEMORY_SPACE_INTF_NAME, &io_memory_interface);
	
	// register access interface (for SkyEye graphical control interface)
	static skyeye_reg_intf reg_interface =
	{
		.get_regvalue_by_id = interface_test_get_regvalue_by_id,
		.get_regname_by_id  = interface_test_get_regname_by_id,
		.set_regvalue_by_id = interface_test_set_regvalue_by_id,
		.get_regnum         = interface_test_get_regnum,
		.get_regid_by_name  = interface_test_get_regid_by_name
	};
	SKY_register_iface(dev_class, SKYEYE_REG_INTF, &reg_interface);
	
	// list of interface methods
	static const struct InterfaceDescription ifaces[] =
	{
		(struct InterfaceDescription)
		{
			.name  = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory_interface
		},
		(struct InterfaceDescription)
		{
			.name  = SKYEYE_REG_INTF,
			.iface = &reg_interface
		},
	};
	
	// list of interface connection methods
	static const struct ConnectDescription connects[] =
	{
		/* ConnectDescription */
	};
	class_register_ifaces(dev_class, ifaces);
	class_register_connects(dev_class, connects);
}
