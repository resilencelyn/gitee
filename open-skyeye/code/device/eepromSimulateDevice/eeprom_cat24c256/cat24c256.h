#ifndef __EEPROM_CAT24C256_
#define __EEPROM_CAT24C256_

#define CAPACITY   0x8000                                /*size of eeprom cat24c256*/

typedef  struct EEPROM_CAT24C256State{
	conf_object_t *obj;
	uint32_t offset;                                 /*location of the last operation */
	uint8_t address;                                 /*i2c slave dev address*/
	uint32_t capacity;                               /*size of eeprom cat24c256*/
	int8_t *imagepath;                               /*where is imagepath of the eeprom*/
	uint32_t addr_count;
	int32_t op_mod;
	uint8_t *mapped_addr;
	uint32_t mapped_size;
	uint32_t addr_l;
	uint32_t addr_h;
}cat24c256_dev;

#endif
