#ifndef __I2C_BUS_H
#define __I2C_BUS_H

#include <skyeye_dev_intf.h>

#define I2C_DEV_MAX_COUNT  127
typedef struct I2CState{
	conf_object_t *obj;                /*i2c bus itself*/
	conf_object_t *current_slave;      /*the current slave communicating with master*/

	conf_object_t* slave_obj;
	i2c_device_intf *slave_iface;          /*the current slave's interface*/

	conf_object_t *slave_device_list[I2C_DEV_MAX_COUNT];       /*all i2c device link the i2c bus*/
	uint32_t slave_count;              /*the count of all i2c device link the i2c bus*/
}i2c_bus_t;


#endif
