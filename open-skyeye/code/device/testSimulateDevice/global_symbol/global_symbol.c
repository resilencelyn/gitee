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
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_system.h>
#include <parse_symbol.h>
#include "global_symbol.h"

static exception_t global_symbol_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	global_symbol_device *dev = opaque->obj;

	return 0;
}

static exception_t global_symbol_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	global_symbol_device *dev = opaque->obj;

	return 0;
}

static void Call_SkyEye_GetSymbol_Addr(char *cpu_name, char *sym_str, uint32_t *global_addr)
{
	SkyEye_GetSymbol_Addr(cpu_name, sym_str, global_addr);

	printf("\n%s:\nglobal_addr: 0x%x\n\n", sym_str, *global_addr);
}

static void Call_SkyEye_GetFunc_Addr(char *cpu_name, char *func_str, uint32_t *func_addr)
{
	*func_addr=SkyEye_GetFunc_Addr(cpu_name, func_str);

	printf("\n%s:\nfunc_addr: 0x%x\n\n", func_str, *func_addr);
}

static void Call_SkyEye_GetFunc_length(char *cpu_name, char *func_str, uint32_t *func_length)
{
	*func_length=SkyEye_GetFunc_Length(cpu_name, func_str);

	printf("\n%s:\nfunc_length: 0x%x\n\n", func_str, *func_length);
}

static int GetSymbol_Addr_Test(char *cpu_name)
{
	uint32_t global_addr = 0;
	uint32_t global_length=0;
	uint32_t err_count   = 0;

	//SkyEye_GetSymbol_Addr("c67x_core_0", "open_file_var.xxx2", &global_addr);
	//printf("\n%s global_addr: 0x%x\n", __func__, global_addr);
	//Call_SkyEye_GetSymbol_Addr("open_file_var.xxx2", &global_addr);

    parse_xml_output();

	Call_SkyEye_GetFunc_Addr(cpu_name, "main", &global_addr);
	Call_SkyEye_GetFunc_length(cpu_name, "main", &global_length);
	if (global_addr != 0x700) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}
	
	Call_SkyEye_GetFunc_Addr(cpu_name, "test_func", &global_addr);
	Call_SkyEye_GetFunc_length(cpu_name, "test_func", &global_length);
	if (global_addr != 0x6aa0) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	Call_SkyEye_GetFunc_Addr(cpu_name, "fputs", &global_addr);
	Call_SkyEye_GetFunc_length(cpu_name, "fputs", &global_length);
	if (global_addr != 0x130) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	Call_SkyEye_GetSymbol_Addr(cpu_name, "test_ok_flags", &global_addr);
	if (global_addr != 0x89f4) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	Call_SkyEye_GetSymbol_Addr(cpu_name, "test_var1->xmvar1[2].dxx", &global_addr);
	if (global_addr != 0x7444) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	Call_SkyEye_GetSymbol_Addr(cpu_name, "test_var1[2].xmvar1[2].dxx", &global_addr);
	if (global_addr != 0x74f4) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	Call_SkyEye_GetSymbol_Addr(cpu_name, "test_var2[2].xmvar2[2].dxx", &global_addr);
	if (global_addr != 0x7654) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	Call_SkyEye_GetSymbol_Addr(cpu_name, "test_var3.xmvar3.dxx", &global_addr);
	if (global_addr != 0x8874) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	Call_SkyEye_GetSymbol_Addr(cpu_name, "test_var4.xmvar4.dxx[2]", &global_addr);
	if (global_addr != 0x888c) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	Call_SkyEye_GetSymbol_Addr(cpu_name, "test_var5->xmvar5->dxx", &global_addr);
	if (global_addr != 0x77ac) {
		err_count++;
		printf("Test Error\n");
	} else {
		printf("Test Ok\n");
	}

	return err_count;
}

static void Call_SkyEye_GetSymbol_Value(char *cpu_name, char *sym_str, void *global_value, size_t count)
{
	SkyEye_GetSymbol_Value(cpu_name, sym_str, global_value, count);

	printf("\n%s:\nglobal_value: %d\n\n", sym_str, *(uint32_t *)global_value);
}

static int GetSymbol_Value_Test(char *cpu_name)
{
	uint32_t global_value = 0;
	uint32_t err_count    = 0;

	Call_SkyEye_GetSymbol_Value(cpu_name, "test_var1[2].xmvar1[2].dxx", GV_VAL_PTR(global_value), 4);
	if (global_value == 1234) {
		printf("Test Ok\n");
	} else {
		printf("Test Error\n");
		err_count++;
	}

	Call_SkyEye_GetSymbol_Value(cpu_name, "test_var2[2].xmvar2[2].dxx", GV_VAL_PTR(global_value), 4);
	if (global_value == 5678) {
		printf("Test Ok\n");
	} else {
		printf("Test Error\n");
	}

	Call_SkyEye_GetSymbol_Value(cpu_name, "test_var3.xmvar3.dxx", GV_VAL_PTR(global_value), 4);
	if (global_value == 8844) {
		printf("Test Ok\n");
	} else {
		printf("Test Error\n");
		err_count++;
	}

	Call_SkyEye_GetSymbol_Value(cpu_name, "test_var4.xmvar4.dxx[2]", GV_VAL_PTR(global_value), 4);
	if (global_value == 6633) {
		printf("Test Ok\n");
	} else {
		printf("Test Error\n");
		err_count++;
	}

	Call_SkyEye_GetSymbol_Value(cpu_name, "test_var5->xmvar5->dxx", GV_VAL_PTR(global_value), 4);
	if (global_value == 6677) {
		printf("Test Ok\n");
	} else {
		printf("Test Error\n");
		err_count++;
	}

	return err_count;
}

static void timer_func(void *obj) {
	global_symbol_device *dev = ((conf_object_t *)obj)->obj;

	//printf("%s %s\n", __FILE__, __func__);

	static uint32_t count = 0;

	count++;

	if (count == 20) {
		uint32_t global_value = 0;
		uint32_t global_addr = 0;
		int ret;

		////////////////////////////////////////////////////////////////////////
		printf("\n\n====================================\n");
		printf("    Parse Symbol Test: SkyEye_GetSymbol_Addr()\n\n");

		ret = GetSymbol_Addr_Test("c67x_core_0");
		if (ret != 0) {
			printf("SkyEye_GetSymbol_Addr Test, %d wrong tests\n", ret);
		}
		else
		{
			printf("SkyEye_GetSymbol_Addr Test All Pass!\n");
		}

		//////////////////////////////////////////////////////////////////////////
		printf("\n\n====================================\n");
		printf("    Parse Symbol Test: SkyEye_GetSymbol_Value()\n\n");

		ret = GetSymbol_Value_Test("c67x_core_0");
		if (ret != 0) {
			printf("SkyEye_GetSymbol_Value Test, %d wrong tests\n", ret);
		}
		else
		{
			printf("SkyEye_GetSymbol_Value Test All Pass!\n");
		}

		//////////////////////////////////////////////////////////////////////////
		printf("\n\n====================================\n");
		printf("    Parse Symbol Test: SkyEye_SetSymbol_Value()\n\n");

		SkyEye_GetSymbol_Value("c67x_core_0", "test_var1[2].xmvar1[2].dxx", GV_VAL_PTR(global_value), 4);
		printf("global_value: %d\n", global_value);

		global_value = 9999;
		SkyEye_SetSymbol_Value("c67x_core_0", "test_var1[2].xmvar1[2].dxx", GV_VAL_PTR(global_value), 4);

		SkyEye_GetSymbol_Value("c67x_core_0", "test_var1[2].xmvar1[2].dxx", GV_VAL_PTR(global_value), 4);
		printf("global_value: %d\n", global_value);

		SkyEye_GetSymbol_Addr("c67x_core_0", "gl_db_data", &global_addr);
		printf("global_addr: 0x%x\n", global_addr);

		//////////////////////////////////////////////////////////////////////////
		printf("\n\n====================================\n");
		printf("    Parse double type Test\n\n");
		double gl_db_data, gl_db_data2;

		SkyEye_GetSymbol_Value("c67x_core_0", "gl_db_data", GV_VAL_PTR(gl_db_data), GV_BIT64);

		printf("gl_db_data: %lf\n", gl_db_data);

		gl_db_data2 = 789.93;
		printf("gl_db_data2: %lf\n", gl_db_data2);

		SkyEye_SetSymbol_Value("c67x_core_0", "gl_db_data", GV_VAL_PTR(gl_db_data2), GV_BIT64);
		SkyEye_GetSymbol_Value("c67x_core_0", "gl_db_data", GV_VAL_PTR(gl_db_data), GV_BIT64);
		printf("gl_db_data: %lf\n", gl_db_data);


		//////////////////////////////////////////////////////////////////////////
		printf("\n\n====================================\n");
		printf("    Parse Symbol Error Test\n\n");

		sym_info_t info;
		info = SkyEye_GetSymbol_Addr("c67x_core_0", "test_var1[2xxx].xmvar1[2].dxx", &global_addr);
		printf("info %d global_addr: 0x%x\n", info, global_addr);

		const char *msg;
		SkyEye_GetSymbol_GetErrorMsg(info, &msg);
		printf("[Msg]: %s\n", msg);

		//////////////////////////////////////////////////////////////////////////
		// Test Ok
		printf("\n\n====================================\n");
		printf("    write test_ok_flags = 1, Test OK!\n\n");

		global_value = 1;
		SkyEye_SetSymbol_Value("c67x_core_0", "test_ok_flags", GV_VAL_PTR(global_value), 4);

		SkyEye_GetSymbol_Value("c67x_core_0", "test_ok_flags", GV_VAL_PTR(global_value), 4);
		printf("global_value: %d\n", global_value);
	}
}

static char* global_symbol_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}

static exception_t global_symbol_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	global_symbol_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}

static uint32_t global_symbol_get_regnum(conf_object_t *conf_obj)
{
	//return sizeof(struct registers) / 4;
	return 0;
}

static uint32_t global_symbol_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	global_symbol_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}

static uint32_t global_symbol_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_global_symbol(const char *obj_name)
{
	global_symbol_device* dev = skyeye_mm_zero(sizeof(global_symbol_device));
	dev->obj = new_conf_object(obj_name, dev);
	struct registers *regs = &(dev->regs);

	system_register_timer_handler(dev->obj, 5000, (time_sched_t)timer_func, SCHED_US|SCHED_NORMAL);

	return dev->obj;
}

static exception_t free_global_symbol(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

void init_global_symbol(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "global_symbol",
		.class_desc = "global_symbol",
		.new_instance = new_global_symbol,
		.free_instance = free_global_symbol
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static memory_space_intf io_memory =
	{
		.read = global_symbol_read,
		.write = global_symbol_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = global_symbol_get_regvalue_by_id,
		.get_regname_by_id = global_symbol_get_regname_by_id,
		.set_regvalue_by_id = global_symbol_set_regvalue_by_id,
		.get_regnum = global_symbol_get_regnum,
		.get_regid_by_name = global_symbol_get_regid_by_name
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const struct InterfaceDescription ifaces[] = 
	{
		(struct InterfaceDescription)
		{
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory
		},
		(struct InterfaceDescription)
		{
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf
		},
	};

	static const struct ConnectDescription connects[] = 
	{
		/* ConnectDescription */
	};

	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);
}
