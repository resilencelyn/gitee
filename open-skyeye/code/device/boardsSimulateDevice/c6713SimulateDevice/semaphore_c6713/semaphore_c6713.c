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
#include "semaphore_c6713.h"

#define printf 

static exception_t semaphore_c6713_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    printf("%s: offset: 0x%x\n", __func__, offset);
	semaphore_c6713_device *dev = opaque->obj;
	switch (offset)
	{
		case SEM_PID:
			//*(uint32_t *)buf = dev->regs.sem_pid;
			*(uint32_t *)buf = 0x48021200;
			break;
		case SEM_RST_RUN:
			*(uint32_t *)buf = dev->regs.sem_rst_run;
			break;
		case SEM_EOI:
			*(uint32_t *)buf = dev->regs.sem_eoi;
			break;
		case SEMERR:
			*(uint32_t *)buf = dev->regs.semerr;
			break;
		case SEMERR_CLEAR:
			*(uint32_t *)buf = dev->regs.semerr_clear;
			break;
		case SEMERR_SET:
			*(uint32_t *)buf = dev->regs.semerr_set;
			break;
		default:
            if ((offset >=  SEM_DIRECT) && (offset < SEMFLAGL)) {
                uint32_t regValue = 0;
                uint32_t regNum = 0;
                regNum = (offset & 0xFF) / 4;
                regValue = dev->sem_r[regNum];

                if ((offset & 0xF00) == 0x300) {
                    printf("----> R300:\n");
                    // 判断信号量是否可用
                    if ((regValue & 0x1) == 1) {
                        *(uint32_t *)buf = 0x1;
                        printf("----> isFree num: %d buf 0x%x regValue 0x%x\n", regNum, *(uint32_t *)buf, regValue);
                    }
                    // 返回Owner
                    if ((regValue & 0x0) == 0) {
                        *(uint32_t *)buf = dev->sem_r[regNum];
                        printf("----> getOwner num: %d buf 0x%x regValue 0x%x\n", regNum, *(uint32_t *)buf, regValue);
                    }
                    printf(">>>>\n\n");
                    break;
                }

                if ((offset & 0xF00) == 0x100) {
                    printf("----> R100:\n");
                    // 获取信号量
                    if ((regValue & 0x1) == 1) {
                        regValue &= ~(0xF0);
                        regValue &= 0;
                        dev->sem_r[regNum] = regValue;
                        *(uint32_t *)buf = 0x1;
                        printf("----> getDirect num: %d buf 0x%x regValue 0x%x\n\n", regNum, *(uint32_t *)buf, regValue);
                        break;
                    // 返回Owner
                    } else if ((regValue & 0x1) == 0x0) {
                        *(uint32_t *)buf = regValue;
                        printf("----> getOwner num: %d buf 0x%x regValue 0x%x\n\n", regNum, *(uint32_t *)buf, regValue);
                        break;
                    }
                    printf("----> read R100 Error\n\n");
                }

                //dev->sem_r[regNum] = regValue;
                //*(uint32_t *)buf = regValue;
                fprintf(stderr, "FUNC:%s Not implemented offset:0x%X\n", __FUNCTION__, offset);
                break;
            } else if ((offset >= SEMFLAGL) && (offset < SEMFLAGL_CLEAR)){
                *(uint32_t *)buf = dev->regs.semflagl[offset / 4];
                break;
            } else if ((offset >= SEMFLAGL_CLEAR) && (offset < SEMFLAGH)) {
                *(uint32_t *)buf = dev->regs.semflagl_clear[offset / 4];
                break;
            } else if ((offset >= SEMFLAGH) && (offset < SEMFLAGH_CLEAR)) {
                *(uint32_t *)buf = dev->regs.semflagh[offset / 4];
                break;
            } else if ((offset >= SEMFLAGH_CLEAR) && (offset < SEMFLAGL_SET)) {
                *(uint32_t *)buf = dev->regs.semflagh_clear[offset / 4];
                break;
            } else if ((offset >= SEMFLAGL_SET) && (offset < SEMFLAGH_SET)) {
                *(uint32_t *)buf = dev->regs.semflagl_set[offset / 4];
                break;
            } else if ((offset >= SEMFLAGH_SET) && (offset < SEMERR)) {
                *(uint32_t *)buf = dev->regs.semflagh_set[offset / 4];
                break;
            } else {
                skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in semaphore_c6713\n", offset);
                break;
            }
	}
	return 0;
}
static exception_t semaphore_c6713_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
	semaphore_c6713_device *dev = opaque->obj;
	uint32_t val = *(uint32_t *)buf;
    printf("\n%s: offset: 0x%x val 0x%X\n", __func__, offset, val);
	switch (offset)
	{
		case SEM_PID:
			dev->regs.sem_pid = val;
			break;
		case SEM_RST_RUN:
			dev->regs.sem_rst_run = val;
			break;
		case SEM_EOI:
			dev->regs.sem_eoi = val;
			break;
		case SEMERR:
			dev->regs.semerr = val;
			break;
		case SEMERR_CLEAR:
			dev->regs.semerr_clear = val;
			break;
		case SEMERR_SET:
			dev->regs.semerr_set = val;
			break;
		default:
            if ((offset >=  SEM_DIRECT) && (offset < SEMFLAGL)) {
                uint32_t regValue = 0;
                uint32_t regNum = 0;
                regNum = (offset & 0xFF) / 4;
                regValue = dev->sem_r[regNum];

                if ((offset & 0xF00) == 0x100) {
                    if ((val >> 31) == 1) {
                        regValue |= val << 8;
                        dev->sem_r[regNum] = regValue;
                        printf("----> wirte Core_id num: %d regValue 0x%x val 0x%x\n\n", regNum, regValue, val);
                        printf(">>>>\n\n");
                        break;
                    }
                    // 释放信号量
                    if ((val & 0x1) == 1) {
                        printf("---->W100 0x1\n");
                        regValue = val;
                        dev->sem_r[regNum] = regValue;
                        printf("----> Free num: %d regValue 0x%x\n", regNum, regValue);
                        if (dev->sem_query_index[regNum] != 0) {
                            uint32_t core_number;
                            core_number = (dev->sem_query[regNum][0] > 8 ) & 0xF;
                            printf("----> core_number 0x%x sem_query[%d][0] 0x%x\n", core_number, regNum, dev->sem_query[regNum][0]);
                            dev->sem_r[regNum] = dev->sem_query[regNum][0];
                            int i, s;
                            for (i = 1; i < dev->sem_query_index[regNum]; i++) {
                                dev->sem_query[regNum][i - 1] = dev->sem_query[regNum][i];
                            }
                            dev->sem_query_index[regNum]--;
                            general_signal_intf* general_signal = dev->general_signal;
                            if (general_signal != NULL)
                                general_signal->event_signal(dev->general_core, dev->event_number,&core_number);
                            printf("----> event_signal event_number 0x%x core_number 0x%x\n", dev->event_number, core_number);
                        }
                        printf(">>>>\n\n");
                        break;
                    }
                    // 添加到队列中
                    if ((val & 0x1) == 0) {
                        printf("---->W100 0x0\n");
                        if ((regValue & 0x1) == 1) {
                            uint32_t core_number;
                            core_number = (val > 8 ) & 0xF;
                            general_signal_intf* general_signal = dev->general_signal;
                            if (general_signal != NULL)
                                general_signal->event_signal(dev->general_core, dev->event_number, &core_number);
                            printf("----> event_signal event_number 0x%x core_number 0x%x\n", dev->event_number, core_number);
                        } else if ((regValue & 0x1) == 0) {
                            dev->sem_query[regNum][dev->sem_query_index[regNum]++] = val;
                            printf("----> Add query sem_query[%d][0] 0x%x val 0x%x\n", dev->sem_query[regNum][0], val);
                        }
                        printf(">>>>\n\n");
                        break;
                    }
                }
                fprintf(stderr, "FUNC:%s Not implemented offset:0x%X\n", __FUNCTION__, offset);
                break;
            } else if ((offset >= SEMFLAGL) && (offset < SEMFLAGL_CLEAR)){
                dev->regs.semflagl[offset / 4] = val;
                printf("%s %d offset 0x%x\n", __FUNCTION__, __LINE__, offset);
                break;
            } else if ((offset >= SEMFLAGL_CLEAR) && (offset < SEMFLAGH)) {
                dev->regs.semflagl_clear[offset / 4] = val;
                printf("%s %d offset 0x%x\n", __FUNCTION__, __LINE__, offset);
                break;
            } else if ((offset >= SEMFLAGH) && (offset < SEMFLAGH_CLEAR)) {
                dev->regs.semflagh[offset / 4] = val;
                printf("%s %d offset 0x%x\n", __FUNCTION__, __LINE__, offset);
                break;
            } else if ((offset >= SEMFLAGH_CLEAR) && (offset < SEMFLAGL_SET)) {
                dev->regs.semflagh_clear[offset / 4] = val;
                printf("%s %d offset 0x%x\n", __FUNCTION__, __LINE__, offset);
                break;
            } else if ((offset >= SEMFLAGL_SET) && (offset < SEMFLAGH_SET)) {
                dev->regs.semflagl_set[offset / 4] = val;
                printf("%s %d offset 0x%x\n", __FUNCTION__, __LINE__, offset);
                break;
            } else if ((offset >= SEMFLAGH_SET) && (offset < SEMERR)) {
                dev->regs.semflagh_set[offset / 4] = val;
                printf("%s %d offset 0x%x\n", __FUNCTION__, __LINE__, offset);
                break;
            } else {
                skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in semaphore_c6713\n", offset);
                break;
            }
	}
	return 0;
}
static char* semaphore_c6713_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
	return regs_name[id];
}
static exception_t semaphore_c6713_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
	semaphore_c6713_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	*((uint32_t*)regs + id) = value;
	return No_exp;
}
static uint32_t semaphore_c6713_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
	return  sizeof(struct registers) / 4;
}
static uint32_t semaphore_c6713_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
	semaphore_c6713_device *dev = conf_obj->obj;
	struct registers *regs = &(dev->regs);
	uint32_t* regs_value = (uint32_t*)regs + id;
	return *regs_value;
}
static uint32_t semaphore_c6713_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
	uint32_t regnum = sizeof(struct registers) / 4, i;
	for(i = 0; i < regnum; i++) {
		if(strcmp(name, regs_name[i]) == 0)
			return i;
	}
	return regnum;
}
static conf_object_t* new_semaphore_c6713(char *obj_name)
{
	semaphore_c6713_device* dev = skyeye_mm_zero(sizeof(semaphore_c6713_device));
	dev->obj = new_conf_object(obj_name, dev);
    int i, j;
    for (i = 0; i < 64; i++) {
        dev->sem_r[i] = 0x1;
    }
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            dev->sem_query[i][j] = 0x0;
        }
        dev->sem_query_index[i] = 0;
    }
	struct registers *regs = &(dev->regs);
	return dev->obj;
}
static exception_t free_semaphore_c6713(conf_object_t* opaque)
{
	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);
	return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	semaphore_c6713_device *dev = obj->obj;
	dev->general_core = obj2;
	dev->general_signal = (general_signal_intf *)SKY_get_iface(dev->general_core, GENERAL_SIGNAL_INTF_NAME);

	if (dev->general_signal == NULL){
		if (dev->general_core == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    semaphore_c6713_device *dev = obj->obj;
    *obj2 = dev->general_core;
    *port = NULL;
    return No_exp;
}

static attr_value_t get_attr_event_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    semaphore_c6713_device *dev = conf_obj->obj;
    attr_value_t event_number = SKY_make_attr_uinteger(dev->event_number);
    return event_number;
}

static exception_t set_attr_event_number(void* arg, conf_object_t* conf_obj,attr_value_t* value, attr_value_t* idx){
    semaphore_c6713_device *dev = conf_obj->obj;
    dev->event_number = SKY_attr_uinteger(*value);
    return No_exp;
}

static class_register_attribute(conf_class_t* clss){
        SKY_register_attribute(clss, "event_number", get_attr_event_number, NULL,set_attr_event_number, NULL, SKY_Attr_Optional, "uinteger", "event_number of c6678");
}

void init_semaphore_c6713(void)
{
	static skyeye_class_t class_data =
	{
		.class_name = "c6713_semaphore",
		.class_desc = "c6713_semaphore",
		.new_instance = new_semaphore_c6713,
		.free_instance = free_semaphore_c6713,
		.module_type = SKYML_ONCHIP(SC_c6713),
	};
	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
	static memory_space_intf io_memory =
	{
		.read = semaphore_c6713_read,
		.write = semaphore_c6713_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
	static skyeye_reg_intf reg_intf =
	{
		.get_regvalue_by_id = semaphore_c6713_get_regvalue_by_id,
		.get_regname_by_id = semaphore_c6713_get_regname_by_id,
		.set_regvalue_by_id = semaphore_c6713_set_regvalue_by_id,
		.get_regnum = semaphore_c6713_get_regnum,
		.get_regid_by_name = semaphore_c6713_get_regid_by_name
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
		(struct ConnectDescription ){
            .name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	class_register_attribute(clss);
}
