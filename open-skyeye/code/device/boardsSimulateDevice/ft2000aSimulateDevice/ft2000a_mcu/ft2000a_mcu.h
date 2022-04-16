#ifndef __FT2000A_MCU_H__
#define __FT2000A_MCU_H__

struct mcu_registers
{
};

struct ft2000a_mcu_device_t
{
	conf_object_t* obj;
	struct mcu_registers regs;
};

typedef struct ft2000a_mcu_device_t ft2000a_mcu_device;

static char* regs_name[] =
{
	NULL
};

static uint32_t regs_offset[] = {
};

#endif
