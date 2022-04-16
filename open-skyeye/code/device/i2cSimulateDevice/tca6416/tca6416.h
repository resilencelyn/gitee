#ifndef __IIC_TCA6416_H
#define __IIC_TCA6416_H
#include <skyeye_dev_bus_intf.h>


typedef struct IICState
{
	conf_object_t *obj;
	conf_object_t* signal_obj;

	struct registers
	{
		int cmd_config_port;
		int cmd_write_port;
	}regs;
	conf_object_t* i2c_bus_obj;
	i2c_bus_intf *i2c_bus_iface;
	int cnt;
	char slave_buf[2];
}tca6416_dev;

#define CMD_CONFIG_PORT 0x6
#define CMD_WRITE_PORT 0x2
#endif
