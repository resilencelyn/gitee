#ifndef __SPI_BUS_H
#define __SPI_BUS_H

#include <skyeye_dev_intf.h>

#define SPI_DEV_MAX_COUNT  127
typedef struct SPIState{
	conf_object_t *obj;                /*spi bus itself*/
	conf_object_t *current_slave;      /*the current slave communicating with master*/
	spi_device_intf *slave_iface;          /*the current slave's interface*/
	conf_object_t *slave_device_list[SPI_DEV_MAX_COUNT];       /*all spi device link the i2c bus*/
	uint32_t slave_count;              /*the count of all spi device link the i2c bus*/
}spi_bus_t;


#endif
