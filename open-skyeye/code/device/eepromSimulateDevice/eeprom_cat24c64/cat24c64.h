#ifndef __EEPROM_CAT24C64_
#define __EEPROM_CAT24C64_

#define CAPACITY   0xFFFF                                /*size of eeprom cat24c256*/

typedef  struct EEPROM_CAT24C64State{
	conf_object_t *obj;
	uint32_t w_offset;                                 /*location of the last operation */
	uint32_t r_offset;                                 /*location of the last operation */
	uint8_t address;                                 /*i2c slave dev address*/
	uint32_t capacity;                               /*size of eeprom cat24c256*/
	int8_t *imagepath;                               /*where is imagepath of the eeprom*/
	uint32_t addr_count;
	int32_t op_mod;
	int dev_id;
	conf_object_t* i2c_bus_obj;
	i2c_bus_intf* i2c_bus_iface;
	uint8_t *mapped_addr;
	uint32_t mapped_size;
	uint32_t addr_l;
	uint32_t addr_h;
}cat24c64_dev;

#endif
